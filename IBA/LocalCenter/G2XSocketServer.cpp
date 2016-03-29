#include "StdAfx.h"
#include "G2XSocketServer.h"

// 每一个处理器上产生多少个线程(为了最大限度的提升服务器性能，详见配套文档)
#define WORKER_THREADS_PER_PROCESSOR 2
// 同时投递的Accept请求的数量(这个要根据实际的情况灵活设置)
#define MAX_POST_ACCEPT              10
// 传递给Worker线程的退出信号
#define EXIT_CODE                    NULL


// 释放指针和句柄资源的宏

// 释放指针宏
#define RELEASE(x)                      {if(x != NULL ){delete x;x=NULL;}}
// 释放句柄宏
#define RELEASE_HANDLE(x)               {if(x != NULL && x!=INVALID_HANDLE_VALUE){ CloseHandle(x);x = NULL;}}
// 释放Socket宏
#define RELEASE_SOCKET(x)               {if(x !=INVALID_SOCKET) { closesocket(x);x=INVALID_SOCKET;}}

#define IS_INVALID_HANDEL(x) (x == NULL || x == INVALID_HANDLE_VALUE)


//-----------------------------------------------------------------------//
// CG2XSocketContext
//-----------------------------------------------------------------------//

CG2XSocketContext::CG2XSocketContext()
{
	m_Socket = INVALID_SOCKET;
	memset(&m_ClientAddr, 0, sizeof(m_ClientAddr));
}

CG2XSocketContext::~CG2XSocketContext()
{
	if( m_Socket!=INVALID_SOCKET )
	{
		closesocket( m_Socket );
		m_Socket = INVALID_SOCKET;
	}
	// 释放掉所有的IO上下文数据
	for( int i=0;i<m_arrayIoContext.GetCount();i++ )
	{
		delete m_arrayIoContext.GetAt(i);
	}
	m_arrayIoContext.RemoveAll();
}
//创建并初始化用于某个IO的连接的信息(socket,overlapped...)，并添加到m_arrayIoContext
CG2XIoContext* CG2XSocketContext::GetNewIoContext()
{
	CG2XIoContext* p = new CG2XIoContext;

	m_arrayIoContext.Add( p );

	return p;
}

void CG2XSocketContext::RemoveContext(CG2XIoContext* pContext)
{
	ASSERT( pContext!=NULL );

	for( int i=0;i<m_arrayIoContext.GetCount();i++ )
	{
		if( pContext==m_arrayIoContext.GetAt(i) )
		{
			delete pContext;
			pContext = NULL;
			m_arrayIoContext.RemoveAt(i);				
			break;
		}
	}
}

//-----------------------------------------------------------------------//
// CG2XSocketServer
//-----------------------------------------------------------------------//

CG2XSocketServer::CG2XSocketServer(CG2XServerSocketDelegate* pDelegate):
							m_nThreads(0),
							m_nAcceptExCount(MAX_POST_ACCEPT),
							m_hShutdownEvent(NULL),
							m_hIOCompletionPort(NULL),
							m_phWorkerThreads(NULL),
							m_nPort(0),
							m_pMain(NULL),
							m_lpfnAcceptEx( NULL ),
							m_pListenContext( NULL )
{
	if (!pDelegate)
	{
		::MessageBoxA(NULL, "CG2XSocketServer 的委托对象的指针不能为空", "ERROR", MB_OK);
		ASSERT(0);
	}
	
	m_pEventDelegate = pDelegate;
}


CG2XSocketServer::~CG2XSocketServer()
{
	// 确保资源彻底释放
	this->Stop();
}

///////////////////////////////////////////////////////////////////
// 工作者线程：  为IOCP请求服务的工作者线程
//         也就是每当完成端口上出现了完成数据包，就将之取出来进行处理的线程
///////////////////////////////////////////////////////////////////
/*************************************************
Function:       //_WorkerThread
Description:    //工作线程的线程函数
				//Note:启动时在_InitializeListenSocket中已经投递了20个AcceptEx请求,因此所有线程函数一开始在没有客户端连进来以前,就会进入GetQueuedCompletionStatus等待
				//有客户端连接时：
				//1.DoAccept调用代理处理第一笔数据的处理,短连接时代理返回false关闭连接重新投递AcceptEx,长连接时绑定连接socket到IOCP并开始投递Recv
				//2.在代理处理时,可选择投递Recv或Send
Calls:          //...
Input:          //lpParam - 线程函数参数(THREADPARAMS_WORKER*)类型:1.SocketServer对象指针 2.线程编号
Return:         //线程结束返回0
*************************************************/
DWORD WINAPI CG2XSocketServer::_WorkerThread(LPVOID lpParam)
{   
	//取出SocketServer指针和线程编号
	THREADPARAMS_WORKER* pParam = (THREADPARAMS_WORKER*)lpParam;
	CG2XSocketServer* pIOCPModel = (CG2XSocketServer*)pParam->pIOCPModel;
	int nThreadNo = (int)pParam->nThreadNo;

	char strText[256] = {0};
	sprintf(strText, "CG2XSocketServer::_InitializeIOCP -> _WorkerThread - Start worker thread %d.\n", nThreadNo);
	pIOCPModel->m_pEventDelegate->didDumpText(strText);

	OVERLAPPED           *pOverlapped = NULL;
	PER_SOCKET_CONTEXT   *pSocketContext = NULL;
	DWORD                dwBytesTransfered = 0;

	// 循环处理请求，直到接收到Shutdown信息为止
	// m_hShutdownEvent初始为无信号状态,进入循环.CG2XSocketServer::Stop()中SetEvent,跳出循环,结束线程,并释放线程线程参数
	while (WAIT_OBJECT_0 != WaitForSingleObject(pIOCPModel->m_hShutdownEvent, 0))
	{
		//从IOCP队列获取CP,没取到前线程在这里会被阻塞住,因设置了永不超时
		BOOL bReturn = GetQueuedCompletionStatus(
			pIOCPModel->m_hIOCompletionPort,
			&dwBytesTransfered,            //接收数据长度
			(PULONG_PTR)&pSocketContext,   //Completion Key,端口传递来的与监听Socket有关的信息
			&pOverlapped,                  //接收Overlapped结构,该结构是在AcceptEx中投递过来的
			INFINITE);                     //永不超时

		// 如果收到的是退出标志，则直接退出
		if ( EXIT_CODE==(DWORD)pSocketContext )
		{
			break;
		}

		// 判断是否出现了错误
		if( !bReturn )  
		{  
			DWORD dwErr = GetLastError();

			// 显示一下提示信息
			if( !pIOCPModel->HandleError( pSocketContext,dwErr ) )
			{
				break;
			}

			continue;  
		}  
		else  
		{  	
			// 读取传入的参数 - PER_IO_CONTEXT,因为投递时的overlapped是在PER_IO_CONTEXT中,所以返回时其也在PER_IO_CONTEXT中,通过这种方式来获得这个结构
			PER_IO_CONTEXT* pIoContext = CONTAINING_RECORD(pOverlapped, PER_IO_CONTEXT, m_Overlapped);  //MS的宏,根据成员pOverlapped的地址获取PER_IO_CONTEXT结构的地址

			// 判断是否有客户端断开了
			//投递的是接收消息或发送消息,调用代理的Disconnected函数,从SocketServer中释放IO信息
			if((0 == dwBytesTransfered) && ( RECV_POSTED==pIoContext->m_OpType || SEND_POSTED==pIoContext->m_OpType))  
			{  
				pIOCPModel->m_pEventDelegate->didClientDisconnected(pIOCPModel, pSocketContext);

				// 释放掉对应的资源
				pIOCPModel->_RemoveContext( pSocketContext );

 				continue;  
			}  
			else
			{
				switch( pIoContext->m_OpType )  
				{  
					 // Accept  
				case ACCEPT_POSTED:   //在_InitializeListenSocket中投递的20个请求都是ACCEPT请求
					{ 
						// 为了增加代码可读性，这里用专门的_DoAccept函数进行处理连入请求
						//取得Accept请求的第一笔数据包并使用代理对象处理,处理完后根据返回值决定是否再投递Recv请求.完成后重新投递Accept请求
						pIOCPModel->_DoAccpet( pSocketContext, pIoContext );//监听和连接Socket Context						
					}
					break;

					// RECV
				case RECV_POSTED:
					{
						// 为了增加代码可读性，这里用专门的_DoRecv函数进行处理接收请求
						pIOCPModel->_DoRecv( pSocketContext,pIoContext ); 
					}
					break;

					// SEND
					// 这里略过不写了，要不代码太多了，不容易理解，Send操作相对来讲简单一些
				case SEND_POSTED:
					{
						pIOCPModel->_DoSend( pSocketContext,pIoContext );
					}
					break;
				default:
					// 不应该执行到这里
					TRACE(_T("_WorkThread中的 pIoContext->m_OpType 参数异常.\n"));
					break;
				} //switch
			}//if
		}//if

	}//while

	TRACE(_T("工作者线程 %d 号退出.\n"),nThreadNo);

	// 释放线程参数
	RELEASE(lpParam);	

	return 0;
}



//====================================================================================
//
//				    系统初始化和终止
//
//====================================================================================




////////////////////////////////////////////////////////////////////
// 初始化WinSock 2.2
/*************************************************
Function:       //Initialize
Description:    //初始化WinSock2.2的DLL库，错误则调用代理的错误处理
Calls:          //m_pEventDelegate->didHandleError
Return:         //错误返回false
*************************************************/
bool CG2XSocketServer::Initialize()
{    
	WSADATA wsaData;
	int nResult;
	nResult = WSAStartup(MAKEWORD(2,2), &wsaData);

	if (NO_ERROR != nResult)
	{
		m_pEventDelegate->didHandleError(this, NULL, CG2XServerSocketDelegate::ERROR_INIT_WINSOCK, GetLastError());
		return false; 
	}

	return true;
}

//////////////////////////////////////////////////////////////////
//	启动服务器
/*************************************************
Function:       //Start
Description:    //创建服务器类.初始化临界资源(用于工作者线程同步和事件委托对象的),创建事件(系统退出的事件通知).
				//创建完成端口,创建工作者线程(ProcessorNo. * 2)
				//创建listen socket,绑定到完成端口,绑定服务器地址和端口,开始监听
				//获取AcceptEx和AcceptExSockAddrs函数指针
				//为创建连接做准备,数量确定是10个连接,为每个连接准备好客户端CONTEXT
Calls:          //...
Input:          //nPort        -- SocketServer的端口
                //nThreadCount - 工作者线程数量
				//nAccecpExCount - 每个端口最多建立的连接数量
Return:         //成功返回true,失败返回false
*************************************************/
bool CG2XSocketServer::Start(const short int nPort,int nThreadCount, int nAccecpExCount)
{
	m_pEventDelegate->didDumpText(GetLocalIP());

	if (!nPort)
	{
		m_pEventDelegate->didDumpText("port is invalid! Start SocketServer failed!");
		return false;
	}
	m_nPort = nPort;
	m_nThreads = nThreadCount;
	m_nAcceptExCount = nAccecpExCount;

	InitializeCriticalSection(&m_csContextList);
	InitializeCriticalSection(&m_csDelegate);

	// 建立系统退出的事件通知
	m_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);//人工复位,初始无信号

	if (IS_INVALID_HANDEL(m_hShutdownEvent))
	{
		m_pEventDelegate->didHandleError(this, NULL, CG2XServerSocketDelegate::ERROR_INIT_STOPEVENT, 0);
		return false;
	}
	//1.创建完成端口 2.创建工作者线程(ProcessorNo. * 2)
	if (false == _InitializeIOCP())
	{
		m_pEventDelegate->didHandleError(this, NULL, CG2XServerSocketDelegate::ERROR_INIT_IOCP, WSAGetLastError());
		return false;
	}
	else
	{
		m_pEventDelegate->didDumpText("Initialize IOCP complete\n");
	}

	//1.创建listen socket,绑定到完成端口,绑定服务器地址和端口,开始监听
	//2.获取AcceptEx和AcceptExSockAddrs函数指针
	//3.为创建连接做准备,数量确定是10个连接,为每个连接准备好客户端CONTEXT
	if( false==_InitializeListenSocket() )
	{
		m_pEventDelegate->didHandleError(this, NULL, CG2XServerSocketDelegate::ERROR_INIT_LISTEN_SOCKET, GetLastError());
		_UnInitialize();
		return false;
	}
	else
	{
		m_pEventDelegate->didDumpText("CG2XSocketServer::Start - Initialize listen socket complete\n");
	}

	m_pEventDelegate->didDumpText("CG2XSocketServer::Start - Ready! Waiting for a connection\n");

	return true;
}


////////////////////////////////////////////////////////////////////
//	开始发送系统退出消息，退出完成端口和线程资源
void CG2XSocketServer::Stop()
{
	if( m_pListenContext!=NULL && m_pListenContext->m_Socket!=INVALID_SOCKET )
	{
		// 激活关闭消息通知
		SetEvent(m_hShutdownEvent); //设置为有信号

		for (int i = 0; i < m_nThreads; i++)
		{
			// 通知所有的完成端口操作退出
			PostQueuedCompletionStatus(m_hIOCompletionPort, 0, (DWORD)EXIT_CODE, NULL);
		}

		// 等待所有的客户端资源退出
		WaitForMultipleObjects(m_nThreads, m_phWorkerThreads, TRUE, INFINITE);

		// 清除客户端列表信息
		this->_ClearContextList();

		// 释放其他资源
		this->_UnInitialize();

		m_pEventDelegate->didDumpText("stop listening!");
	}	
}


////////////////////////////////
// 初始化完成端口
/*************************************************
Function:       //_InitializeIOCP
Description:    //建立一个完成端口，创建处理器数量*2个工作者线程
Return:         //完成端口创建失败返回false,其他返回true
*************************************************/
bool CG2XSocketServer::_InitializeIOCP()
{
	// 建立第一个完成端口
	m_hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0 );

	if ( NULL == m_hIOCompletionPort)
	{
		return false;
	}

	// 根据本机中的处理器数量，建立对应的线程数
	if (m_nThreads <= 0)
		m_nThreads = WORKER_THREADS_PER_PROCESSOR * _GetNoOfProcessors();
	
	// 为工作者线程初始化句柄
	m_phWorkerThreads = new HANDLE[m_nThreads];   
	
	// 根据计算出来的数量建立工作者线程
	DWORD nThreadID;
	for (int i = 0; i < m_nThreads; i++)
	{
		THREADPARAMS_WORKER* pThreadParams = new THREADPARAMS_WORKER;  //传递给线程两个参数-1.SocketServer对象指针 2.线程编号
		pThreadParams->pIOCPModel = this;    //当前socketServer的指针
		pThreadParams->nThreadNo  = i+1;     //线程编号
		m_phWorkerThreads[i] = ::CreateThread(0, 0, _WorkerThread, (void *)pThreadParams, 0, &nThreadID); //线程函数为_WorkerThread,创建后立即激活
	}

	char strText[256] = {0};
	sprintf(strText, "Create %d worker threads\n", m_nThreads);
	m_pEventDelegate->didDumpText(strText); //输出CLocalCenter::dump:后面加字符串

	return true;
}


/////////////////////////////////////////////////////////////////
// 初始化Socket
/*************************************************
Function:       //_InitializeListenSocket
Description:    //1.创建listen socket,绑定到完成端口,绑定服务器地址和端口,开始监听
				//2.获取AcceptEx和AcceptExSockAddrs函数指针
				//3.为创建连接做准备,数量确定是10个连接,为每个连接准备好客户端CONTEXT
Calls:          //...
Return:         //成功返回true
*************************************************/
bool CG2XSocketServer::_InitializeListenSocket()
{
	// AcceptEx 和 GetAcceptExSockaddrs 的GUID，用于导出函数指针
	GUID GuidAcceptEx = WSAID_ACCEPTEX;  
	GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS; 

	// 服务器地址信息，用于绑定Socket
	struct sockaddr_in ServerAddress;

	// 生成用于监听的Socket的信息
	m_pListenContext = new PER_SOCKET_CONTEXT;

	// 需要使用重叠IO，必须得使用WSASocket来建立Socket，才可以支持重叠IO操作
	m_pListenContext->m_Socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == m_pListenContext->m_Socket) 
	{
		char strText[256] = {0};
		sprintf(strText, "error: Initialize socket failure, error code:%d\n", WSAGetLastError());
		m_pEventDelegate->didDumpText(strText, true);
		return false;
	}
	
	// 将Listen Socket绑定至完成端口中
	if( NULL== CreateIoCompletionPort( (HANDLE)m_pListenContext->m_Socket, m_hIOCompletionPort,(DWORD)m_pListenContext, 0))  
	{  
		char strText[256] = {0};
		sprintf(strText, "error: Attach listen socket to IOCP failure, error code:%d\n", WSAGetLastError());
		m_pEventDelegate->didDumpText(strText, true);
 
		RELEASE_SOCKET( m_pListenContext->m_Socket );
		return false;
	}
	
	// 填充地址信息
	ZeroMemory((char *)&ServerAddress, sizeof(ServerAddress));
	ServerAddress.sin_family = AF_INET;
	// 这里可以绑定任何可用的IP地址，或者绑定一个指定的IP地址 
	//ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);                      
	ServerAddress.sin_addr.s_addr = inet_addr(m_strIP);         
	ServerAddress.sin_port = htons(m_nPort);                          

	// 绑定地址
	if (SOCKET_ERROR == bind(m_pListenContext->m_Socket, (struct sockaddr *) &ServerAddress, sizeof(ServerAddress))) 
	{
		char strText[256] = {0};
		sprintf(strText, "error: Binding listen socket failure, error code:%d\n", WSAGetLastError());
		m_pEventDelegate->didDumpText(strText, true);
		return false;
	}

	// 开始进行监听
	if (SOCKET_ERROR == listen(m_pListenContext->m_Socket,SOMAXCONN))
	{
		m_pEventDelegate->didDumpText("error: Listenning failure\n", true);
		return false;
	}

	//获取AcceptEx,AcceptExSockAddrs函数的指针

	// 使用AcceptEx函数，因为这个是属于WinSock2规范之外的微软另外提供的扩展函数
	// 所以需要额外获取一下函数的指针，
	// 获取AcceptEx函数指针
	DWORD dwBytes = 0;  
	if(SOCKET_ERROR == WSAIoctl(
		m_pListenContext->m_Socket, 
		SIO_GET_EXTENSION_FUNCTION_POINTER, 
		&GuidAcceptEx, 
		sizeof(GuidAcceptEx), 
		&m_lpfnAcceptEx, 
		sizeof(m_lpfnAcceptEx), 
		&dwBytes, 
		NULL, 
		NULL))  
	{  
		char strText[256] = {0};
		sprintf(strText, "error: SIO_GET_EXTENSION_FUNCTION_POINTER AcceptEx is NULL ,error code:%d\n", WSAGetLastError());
		m_pEventDelegate->didDumpText(strText, true);

		this->_UnInitialize();
		return false;  
	}  

	// 获取GetAcceptExSockAddrs函数指针，也是同理
	if(SOCKET_ERROR == WSAIoctl(
		m_pListenContext->m_Socket, 
		SIO_GET_EXTENSION_FUNCTION_POINTER, 
		&GuidGetAcceptExSockAddrs,
		sizeof(GuidGetAcceptExSockAddrs), 
		&m_lpfnGetAcceptExSockAddrs, 
		sizeof(m_lpfnGetAcceptExSockAddrs),   
		&dwBytes, 
		NULL, 
		NULL))  
	{  
		char strText[256] = {0};
		sprintf(strText, "error: SIO_GET_EXTENSION_FUNCTION_POINTER GetAcceptExSockAddrs is NULL ,error code:%d\n", WSAGetLastError());
		m_pEventDelegate->didDumpText(strText, true); 
		this->_UnInitialize();
		return false; 
	}  


	// 为AcceptEx 准备参数，然后创建10个socket和connection用于该完成端口
	if (m_nAcceptExCount <= 0)
		m_nAcceptExCount = MAX_POST_ACCEPT;  //此处为10个,根据需要灵活设置

	//为每个连接创建对应的PER_IO_CONTEXT，然后创建Socket和并向完成端口投递一个10个Accept请求
	for( int i=0;i<m_nAcceptExCount;i++ )
	{
		// 新建一个IO_CONTEXT，创建失败则删除信息
		PER_IO_CONTEXT* pAcceptIoContext = m_pListenContext->GetNewIoContext();//创建并初始化用于某个IO的连接的信息(socket,overlapped...)，并添加到m_arrayIoContext

		if( false==this->_PostAccept( pAcceptIoContext ) )
		{
			m_pListenContext->RemoveContext(pAcceptIoContext);
			return false;
		}
	}

	char strText[256] = {0};
	sprintf(strText, "Post %d AcceptEx Requests\n", m_nAcceptExCount);
	m_pEventDelegate->didDumpText(strText);

	return true;
}

////////////////////////////////////////////////////////////
//	最后释放掉所有资源
void CG2XSocketServer::_UnInitialize()
{
	// 删除互斥量
	DeleteCriticalSection(&m_csContextList);
	DeleteCriticalSection(&m_csDelegate);

	// 关闭系统退出事件句柄
	RELEASE_HANDLE(m_hShutdownEvent);

	// 释放工作者线程句柄指针
	for( int i=0;i<m_nThreads;i++ )
	{
		RELEASE_HANDLE(m_phWorkerThreads[i]);
	}
	
	RELEASE(m_phWorkerThreads);

	// 关闭IOCP句柄
	RELEASE_HANDLE(m_hIOCompletionPort);

	// 关闭监听Socket
	RELEASE(m_pListenContext);

	m_pEventDelegate->didDumpText("Release resource completed!\n");

}

//////////////////////////////////////////////////////////////////
// 投递Accept请求
/*************************************************
Function:       //_PostAccept
Description:    //投递AcceptEx请求
Calls:          //WSASocket,AcceptEx
Input:          //pAcceptIoContext - 待连接客户端的Socket的PER_IO_CONTEXT,其中的socket待创建并赋值
Return:         //投递未成功或未取得返回,返回false
Others:         //AcceptEx = 投递 + 接收第一笔数据
*************************************************/
bool CG2XSocketServer::_PostAccept( PER_IO_CONTEXT* pAcceptIoContext )
{
	ASSERT( INVALID_SOCKET!=m_pListenContext->m_Socket );

	// 准备参数
	DWORD dwBytes = 0;  
	pAcceptIoContext->m_OpType = ACCEPT_POSTED;            //标志Accept操作
	WSABUF *p_wbuf   = &pAcceptIoContext->m_wsaBuf;        //WSABUF为空
	OVERLAPPED *p_ol = &pAcceptIoContext->m_Overlapped;    //Overlapped为空
	
	// 为以后新连入的客户端先准备好Socket( 这个是与传统accept最大的区别 ) 
	pAcceptIoContext->m_sockAccept  = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);  //这里需要标志WSA_FLAG_OVERLAPPED结构
	if( INVALID_SOCKET==pAcceptIoContext->m_sockAccept )  
	{  
		m_pEventDelegate->didDumpText("post accept error! WSASocket return invalid socket", true); 
		return false;  
	} 

	// 投递AcceptEx,创建好连接,第一部分的数据长度为：总长度- 两个地址的长度(api指定),数据长度指定为0
	if(FALSE == m_lpfnAcceptEx( m_pListenContext->m_Socket, pAcceptIoContext->m_sockAccept, p_wbuf->buf, p_wbuf->len - ((sizeof(SOCKADDR_IN)+16)*2),   
								sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16, &dwBytes, p_ol))  
	{  
		if(WSA_IO_PENDING != WSAGetLastError())  
		{  
			m_pEventDelegate->didDumpText("post acceptEx error!", true); 
			return false;  
		}  
	} 

	return true;
}

/*************************************************
Function:       //_DoAccpet
Description:    //取得Accept请求的第一笔数据包并使用代理对象处理,处理完后根据返回值决定是否再投递Recv请求.(返回false表示短连接,直接关闭连接.否则表示长连接)
				//将连接socket与完成端口关联.完成后重新投递Accept请求
Calls:          //GetAcceptExSockAddrs,m_pEventDelegate->didClientConnected,m_pEventDelegate->didRecvData
Input:          //PER_SOCKET_CONTEXT* pSocketContext - 监听Socket的上下文
                //PER_IO_CONTEXT* pListenContext     - 待连接Socket的上下文
Return:         //m_pEventDelegate->didClientConnected返回false,或者中途出错,返回false.
Others:         //注意每个操作都需要自己的overlap,因此PER_IO_CONTEXT不能共用
				//完成的socket绑定后就成了长连接
*************************************************/
bool CG2XSocketServer::_DoAccpet( PER_SOCKET_CONTEXT* pSocketContext, PER_IO_CONTEXT* pListenContext )
{
	SOCKADDR_IN* ClientAddr = NULL;
	SOCKADDR_IN* LocalAddr = NULL;  
	int remoteLen = sizeof(SOCKADDR_IN), localLen = sizeof(SOCKADDR_IN);  

	///////////////////////////////////////////////////////////////////////////
	// 1. 首先取得连入客户端的地址信息
	//////////////////////////////////////////////////////////////////////////
	this->m_lpfnGetAcceptExSockAddrs(pListenContext->m_wsaBuf.buf, 
		pListenContext->m_wsaBuf.len - ((sizeof(SOCKADDR_IN)+16)*2),  //数据部分的长度
		sizeof(SOCKADDR_IN)+16, 
		sizeof(SOCKADDR_IN)+16, 
		(LPSOCKADDR*)&LocalAddr, 
		&localLen, 
		(LPSOCKADDR*)&ClientAddr,
		&remoteLen);  

	
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// 2. 使用代理类处理连接成功和接收第一笔数据.长连接则将连接的socket与IOCP绑定.
	//		Note:由于在连接Socket的Context还要用于下一个AcceptEx投递,而overlap结构只能对应单次操作,因此我们
	//         复制一个新的Context用于当次连接.
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	PER_SOCKET_CONTEXT* pNewSocketContext = new PER_SOCKET_CONTEXT;
	pNewSocketContext->m_Socket           = pListenContext->m_sockAccept;  //用于连接的socket
	memcpy(&(pNewSocketContext->m_ClientAddr), ClientAddr, sizeof(SOCKADDR_IN));//CONTEXT.m_ClientAddr = 客户端地址
	
	bool bRes = false;
	
	//为了保证工作线程处理数据时,不会因为并发操作其它IBA中的对象而导致一些错误,这里还是使用临界资源比较保险.
	EnterCriticalSection(&m_csDelegate);
		
	//Note:不需要接收第二笔数据的情况  ---   请返回false,已方便下面进行 释放socket,并重新投递下一个AcceptEx.
	bRes = m_pEventDelegate->didClientConnected(this, pNewSocketContext, pListenContext->m_wsaBuf.buf, pListenContext->m_Overlapped.InternalHigh); 
	LeaveCriticalSection(&m_csDelegate);

	//又要投递新的AcceptEx接着去站岗
	if (!bRes)
	{
		RELEASE( pNewSocketContext );  //删除socket及其相关信息，关闭连接

		// 投递下一个AcceptEx请求
		this->_PostAccept( pListenContext );
		return false;
	}


	// 将连接的sokcet和完成端口绑定 -- 走到这里说明是需要再接收第二笔数据了
	if( false==this->_AssociateWithIOCP( pNewSocketContext ) )
	{
		EnterCriticalSection(&m_csDelegate);
		m_pEventDelegate->didHandleError(this, pNewSocketContext, CG2XServerSocketDelegate::ERROR_ATTACH_IOCP, GetLastError());
		LeaveCriticalSection(&m_csDelegate);

		RELEASE( pNewSocketContext );
		return false;
	}  

	//数据存在,进行数据处理.貌似与didClientConnected有些冲突.已经在didClientConnected中进行处理了,这里又重复进行处理？           
	if (pListenContext->m_Overlapped.InternalHigh > 0)   //I/O端口传送的数据长度在overlapped结构中.
	{
		m_pEventDelegate->didRecvData(this, pNewSocketContext, pListenContext->m_wsaBuf.buf, pListenContext->m_Overlapped.InternalHigh);
	}


	///////////////////////////////////////////////////////////////////////////////////////////////////
	// 3. 继续，建立其下的IoContext，用于在这个Socket上投递第一个Recv数据请求
	///////////////////////////////////////////////////////////////////////////////////////////////////
	PER_IO_CONTEXT* pNewIoContext = pNewSocketContext->GetNewIoContext();
	pNewIoContext->m_OpType       = RECV_POSTED;
	pNewIoContext->m_sockAccept   = pNewSocketContext->m_Socket;
	// 如果Buffer需要保留，就自己拷贝一份出来
	//memcpy( pNewIoContext->m_szBuffer,pIoContext->m_szBuffer,MAX_BUFFER_LEN );

	// 绑定完毕之后，就可以开始在这个Socket上投递完成请求了
	if( false==this->_PostRecv( pNewIoContext) )
	{
		pNewSocketContext->RemoveContext( pNewIoContext );
		return false;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////
	// 4. 如果投递成功，那么就把这个有效的客户端信息，加入到ContextList中去(需要统一管理，方便释放资源)
	this->_AddToContextList( pNewSocketContext );

	////////////////////////////////////////////////////////////////////////////////////////////////
	// 5. 使用完毕之后，把Listen Socket的IoContext的数据缓冲区重置，然后准备投递新的AcceptEx
	pListenContext->ResetBuffer();     //只是重置缓冲区内容，并没有关闭连接
	return this->_PostAccept( pListenContext ); 	
}

////////////////////////////////////////////////////////////////////
// 投递接收数据请求
/*************************************************
Function:       //_PostRecv
Description:    //投递Recv请求
Calls:          //WSARecv
Input:          //PER_IO_CONTEXT* pIoContext - 用于投递请求的Socket上下文
Return:         //成功返回true
*************************************************/
bool CG2XSocketServer::_PostRecv( PER_IO_CONTEXT* pIoContext )
{
	// 初始化变量
	DWORD dwFlags = 0;
	DWORD dwBytes = 0;
	WSABUF *p_wbuf   = &pIoContext->m_wsaBuf;
	OVERLAPPED *p_ol = &pIoContext->m_Overlapped;

	pIoContext->ResetBuffer();
	pIoContext->m_OpType = RECV_POSTED;

	// 初始化完成后，投递WSARecv请求
	int nBytesRecv = WSARecv( pIoContext->m_sockAccept, p_wbuf, 1, &dwBytes, &dwFlags, p_ol, NULL );

	// 如果返回值错误，并且错误的代码并非是Pending的话，那就说明这个重叠请求失败了
	if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
	{
		m_pEventDelegate->didDumpText("post the first WSARecv failure unsuccessfully!", true);
		return false;
	}
	return true;
}


/////////////////////////////////////////////////////////////////
// 在有接收的数据到达的时候，进行处理
/*************************************************
Function:       //_DoRecv
Description:    //处理Recv的I/O请求
Calls:          //m_pEventDelegate->didRecvData
Input:          //PER_SOCKET_CONTEXT* pSocketContext  -- 监听socket上下文
                //PER_IO_CONTEXT* pIoContext          -- 连接socket上下文
Return:         //成功返回true
*************************************************/
bool CG2XSocketServer::_DoRecv( PER_SOCKET_CONTEXT* pSocketContext, PER_IO_CONTEXT* pIoContext )
{
	// 先把上一次的数据显示出现，然后就重置状态，发出下一个Recv请求

	m_pEventDelegate->didRecvData(this, pSocketContext, pIoContext->m_wsaBuf.buf, pIoContext->m_Overlapped.InternalHigh);

	// 然后开始投递下一个WSARecv请求
	return _PostRecv( pIoContext );
}


int CG2XSocketServer::SendData(PER_SOCKET_CONTEXT* pClient, const void* pData, const int nDataLen)
{
	if (nDataLen > MAX_BUFFER_LEN){
		ASSERT(0);
		return 0;
	}

	PER_IO_CONTEXT* pNewIoContext = pClient->GetNewIoContext();
	pNewIoContext->m_OpType       = SEND_POSTED;
	pNewIoContext->m_sockAccept   = pClient->m_Socket;
	memcpy(pNewIoContext->m_szBuffer, pData, nDataLen);
	pNewIoContext->m_Overlapped.InternalHigh = nDataLen;

	//int nRes = send(pClient->m_Socket, pNewIoContext->m_wsaBuf.buf, pNewIoContext->m_Overlapped.InternalHigh, 0);

	// 如果Buffer需要保留，就自己拷贝一份出来
	//memcpy( pNewIoContext->m_szBuffer,pIoContext->m_szBuffer,MAX_BUFFER_LEN );

	// 初始化变量
	DWORD dwFlags = 0;
	DWORD dwBytes = 0;
	WSABUF *p_wbuf   = &pNewIoContext->m_wsaBuf;
	OVERLAPPED *p_ol = &pNewIoContext->m_Overlapped;
	p_wbuf->len = nDataLen;

	pNewIoContext->m_OpType = SEND_POSTED;

	// 投递发送数据
	int nSend = WSASend( pNewIoContext->m_sockAccept, p_wbuf, 1, &dwBytes, dwFlags, p_ol, NULL );

	// 如果返回值错误，并且错误的代码并非是Pending的话，那就说明这个重叠请求失败了
	if ((SOCKET_ERROR == nSend) && (WSA_IO_PENDING != WSAGetLastError()))
	{
		m_pEventDelegate->didHandleError(this, pClient, CG2XServerSocketDelegate::ERROR_SEND_ERROR, WSAGetLastError());
		return -1;
	}
	return nSend;
}

/*************************************************
Function:       //_DoSend
Description:    //处理Recv的I/O请求
Calls:          //m_pEventDelegate->didSendCompleted
Input:          //PER_SOCKET_CONTEXT* pSocketContext  -- 监听socket上下文
				//PER_IO_CONTEXT* pIoContext          -- 连接socket上下文
Return:         //成功返回true
*************************************************/
bool CG2XSocketServer::_DoSend(PER_SOCKET_CONTEXT* pSocketContect, PER_IO_CONTEXT* pIoContext)
{
	//int nRes = send(pSocketContect->m_Socket, pIoContext->m_wsaBuf.buf, pIoContext->m_Overlapped.InternalHigh, 0);
	//return nRes != SOCKET_ERROR;
	m_pEventDelegate->didSendCompleted(this, pSocketContect, pIoContext->m_szBuffer, pIoContext->m_Overlapped.InternalHigh);
	return true;
}


/////////////////////////////////////////////////////
// 将句柄(Socket)绑定到完成端口中
/*************************************************
Function:       //_AssociateWithIOCP
Description:    //将与客户端通信的SOCKET绑定到IOCP
Calls:          //CreateIoCompletionPort
Input:          //PER_SOCKET_CONTEXT *pContext - 连接的socket的上下文
Return:         //成功返回true
*************************************************/
bool CG2XSocketServer::_AssociateWithIOCP( PER_SOCKET_CONTEXT *pContext )
{
	// 将用于和客户端通信的SOCKET绑定到完成端口中
	HANDLE hTemp = CreateIoCompletionPort((HANDLE)pContext->m_Socket, m_hIOCompletionPort, (DWORD)pContext, 0);

	if (NULL == hTemp)
	{
		m_pEventDelegate->didDumpText("CreateIoCompletionPort() error!", true);
		m_pEventDelegate->didHandleError(this, m_pListenContext, CG2XServerSocketDelegate::ERROR_INIT_IOCP, GetLastError());
		return false;
	}

	return true;
}


//====================================================================================
//
//				    ContextList 相关操作
//
//====================================================================================


//////////////////////////////////////////////////////////////
// 将客户端的相关信息存储到数组中
void CG2XSocketServer::_AddToContextList( PER_SOCKET_CONTEXT *pHandleData )
{
	EnterCriticalSection(&m_csContextList);

	m_arrayClientContext.Add(pHandleData);	
	
	LeaveCriticalSection(&m_csContextList);
}

////////////////////////////////////////////////////////////////
//	移除某个特定的Context
void CG2XSocketServer::_RemoveContext( PER_SOCKET_CONTEXT *pSocketContext )
{
	EnterCriticalSection(&m_csContextList);

	for( int i=0;i<m_arrayClientContext.GetCount();i++ )
	{
		if( pSocketContext==m_arrayClientContext.GetAt(i) )
		{
			RELEASE( pSocketContext );			
			m_arrayClientContext.RemoveAt(i);			
			break;
		}
	}

	LeaveCriticalSection(&m_csContextList);
}

////////////////////////////////////////////////////////////////
// 清空客户端信息
void CG2XSocketServer::_ClearContextList()
{
	EnterCriticalSection(&m_csContextList);

	for( int i=0;i<m_arrayClientContext.GetCount();i++ )
	{
		delete m_arrayClientContext.GetAt(i);
	}

	m_arrayClientContext.RemoveAll();

	LeaveCriticalSection(&m_csContextList);
}



//====================================================================================
//
//				       其他辅助函数定义
//
//====================================================================================



////////////////////////////////////////////////////////////////////
// 获得本机的IP地址
/*************************************************
Function:       //GetLocalIP
Description:    //取得本机的IP地址列表中的第一个地址
Calls:          //gethostname，gethostbyname
Return:         //char* m_strIP -- IP地址字符串
*************************************************/
char* CG2XSocketServer::GetLocalIP()
{
	// 获得本机主机名
	char hostname[MAX_PATH] = {0};
	gethostname(hostname,MAX_PATH);                
	struct hostent FAR* lpHostEnt = gethostbyname(hostname); //返回包含地址名和地址列表的hostent结构体
	if(lpHostEnt == NULL)
	{
		return DEFAULT_IP;
	}

	// 取得IP地址列表中的第一个为返回的IP(因为一台主机可能会绑定多个IP)
	LPSTR lpAddr = lpHostEnt->h_addr_list[0];      

	// 将IP地址转化成字符串形式
	struct in_addr inAddr;
	memmove(&inAddr,lpAddr,4);
	strcpy(m_strIP,inet_ntoa(inAddr));        

	return m_strIP;
}

///////////////////////////////////////////////////////////////////
// 获得本机中处理器的数量
int CG2XSocketServer::_GetNoOfProcessors()
{
	SYSTEM_INFO si;

	GetSystemInfo(&si);

	return si.dwNumberOfProcessors;
}


/////////////////////////////////////////////////////////////////////
// 判断客户端Socket是否已经断开，否则在一个无效的Socket上投递WSARecv操作会出现异常
// 使用的方法是尝试向这个socket发送数据，判断这个socket调用的返回值
// 因为如果客户端网络异常断开(例如客户端崩溃或者拔掉网线等)的时候，服务器端是无法收到客户端断开的通知的

bool CG2XSocketServer::_IsSocketAlive(SOCKET s)
{
	int nByteSent=send(s,"",0,0);
	if (-1 == nByteSent) return false;
	return true;
}

///////////////////////////////////////////////////////////////////
// 显示并处理完成端口上的错误
bool CG2XSocketServer::HandleError( PER_SOCKET_CONTEXT *pContext,const DWORD& dwErr )
{
	return true;

	//// 如果是超时了，就再继续等吧  
	//if(WAIT_TIMEOUT == dwErr)  
	//{  	
	//	// 确认客户端是否还活着...
	//	if( !_IsSocketAlive( pContext->m_Socket) )
	//	{
	//		this->m_pEventDelegate->didClientExceptExit(pContext);
	//		this->_RemoveContext( pContext );
	//		return true;
	//	}
	//	else
	//	{
	//		this->_ShowMessage( _T("网络操作超时！重试中...") );
	//		return true;
	//	}
	//}  

	//// 可能是客户端异常退出了
	//else if( ERROR_NETNAME_DELETED==dwErr )
	//{
	//	this->_ShowMessage( _T("检测到客户端异常退出！") );
	//	this->_RemoveContext( pContext );
	//	return true;
	//}

	//else
	//{
	//	this->_ShowMessage( _T("完成端口操作出现错误，线程退出。错误代码：%d"),dwErr );
	//	return false;
	//}
}




