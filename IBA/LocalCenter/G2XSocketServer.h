#pragma once

// winsock 2 的头文件和库
#include <winsock2.h>
#include <MSWSock.h>
#pragma comment(lib,"ws2_32.lib")
#include <afxtempl.h>
#include <string>

// 缓冲区长度 (1024*8)
// 之所以为什么设置8K，也是一个江湖上的经验值
// 如果确实客户端发来的每组数据都比较少，那么就设置得小一些，省内存
#define MAX_BUFFER_LEN        8192  
// 默认端口
#define DEFAULT_PORT          12345    
// 默认IP地址
#define DEFAULT_IP            "127.0.0.1"


//////////////////////////////////////////////////////////////////
// 在完成端口上投递的I/O操作的类型
typedef enum _OPERATION_TYPE  
{  
	ACCEPT_POSTED,                     // 标志投递的Accept操作
	SEND_POSTED,                       // 标志投递的是发送操作
	RECV_POSTED,                       // 标志投递的是接收操作
	NULL_POSTED                        // 用于初始化，无意义

}OPERATION_TYPE;

//====================================================================================
//
//				单IO数据结构体定义(用于每一个重叠操作的参数)
//
//====================================================================================

typedef struct CG2XIoContext
{
	OVERLAPPED     m_Overlapped;                               // 每一个重叠网络操作的重叠结构(针对每一个Socket的每一个操作，都要有一个)              
	SOCKET         m_sockAccept;                               // 这个网络操作所使用的Socket
	WSABUF         m_wsaBuf;                                   // WSA类型的缓冲区，用于给重叠操作传参数的
	char           m_szBuffer[MAX_BUFFER_LEN];                 // 这个是WSABUF里具体存字符的缓冲区,这里为8KB
	OPERATION_TYPE m_OpType;                                   // 标识网络操作的类型(对应上面的枚举)

	// 初始化
	CG2XIoContext()
	{
		ZeroMemory(&m_Overlapped, sizeof(m_Overlapped));  
		ZeroMemory( m_szBuffer,MAX_BUFFER_LEN );
		m_sockAccept = INVALID_SOCKET;
		m_wsaBuf.buf = m_szBuffer;
		m_wsaBuf.len = MAX_BUFFER_LEN;   //WSA缓冲区大小初始化为8KB
		m_OpType     = NULL_POSTED;
	}
	// 释放掉Socket
	~CG2XIoContext()
	{
		if( m_sockAccept!=INVALID_SOCKET )
		{
			closesocket(m_sockAccept);
			m_sockAccept = INVALID_SOCKET;
		}
	}
	// 重置缓冲区内容
	void ResetBuffer()
	{
		ZeroMemory( m_szBuffer,MAX_BUFFER_LEN );
	}

} PER_IO_CONTEXT;


//====================================================================================
//
//	每一个新客户端连接，对应一个CG2XSocketContext对象
//
//====================================================================================
class CG2XSocketServer;
typedef class CG2XSocketContext
{  
public:
	CG2XSocketContext();
	virtual ~CG2XSocketContext();

private:
	friend class CG2XSocketServer;
	SOCKET      m_Socket;									// Client Socket
	SOCKADDR_IN m_ClientAddr;								// Client Address
	CArray<PER_IO_CONTEXT*> m_arrayIoContext;				// 客户端网络操作的上下文数据，也就是说对于每一个客户端Socket，是可以在上面同时投递多个IO请求的
	DWORD m_dataOrDataPtr;									// 自定义的客户端数据(一般可以用来标识客户端)

protected:
	/* 
		Create one new CG2XIoContext, pushing it back to m_arrayIoContext, 
		return object's pointer.
	*/
	CG2XIoContext* GetNewIoContext();

	/*
		Remove CG2XIoContext object from m_arrayIoContext.
		Can't input NULL!
	*/
	void RemoveContext( CG2XIoContext* pContext );

public:

	/* Get IP Address */
	std::string GetAddress() { 
		return std::string(inet_ntoa(m_ClientAddr.sin_addr)); 
	}
	
	unsigned short GetPort() {
		return ntohs(m_ClientAddr.sin_port);
	}

	void SetData(DWORD data){ m_dataOrDataPtr = data; }

	DWORD GetData() { return m_dataOrDataPtr; }

}PER_SOCKET_CONTEXT;


// 工作者线程的线程参数;
class CG2XServerSocketDelegate
{
	friend class CG2XSocketServer;

public:
	enum ERROR_CODE
	{
		ERROR_NO = 0,
		ERROR_INIT_WINSOCK,			 // 初始化WinSock2.2失败
		ERROR_INIT_STOPEVENT,		 // 服务器停止事件失败
		ERROR_INIT_IOCP,			 // 初始化完成端口失败
		ERROR_INIT_LISTEN_SOCKET,	 // 初始化监听Socket失败
		ERROR_ATTACH_IOCP,			 // 接入的客户端socket与完成端口绑定失败
		//ERROR_CLIENT_EXCEPT_EXIT	 // 客户端异常退出
		ERROR_SEND_ERROR,			 // 发送数据出错
	};

protected:
	// 处理客户端的连入事件,并接收客户端发送的第一组数据 如果返回false,那么则阻止客户端的介入
	virtual bool didClientConnected(CG2XSocketServer* pServer, PER_SOCKET_CONTEXT* pClient, const char* pData, const int nBufLen) { return true; }

	// 处理客户端的异常退出事件
	virtual void didClientExceptExit(CG2XSocketServer* pServer, PER_SOCKET_CONTEXT* pClient) {}

	// 处理错误信息
	virtual bool didHandleError(CG2XSocketServer* pServer, PER_SOCKET_CONTEXT* pClient, ERROR_CODE code, DWORD dwLastErrorCode) { return false; }

	// 处理接收数据
	virtual void didRecvData(CG2XSocketServer* pServer, PER_SOCKET_CONTEXT* pClient, const char* pData, const int nBufLen) {};

	// 客户端断开连接
	virtual void didClientDisconnected(CG2XSocketServer* pServer, PER_SOCKET_CONTEXT* pClient) {}

	// 数据发送完毕
	virtual void didSendCompleted(CG2XSocketServer* pServer, PER_SOCKET_CONTEXT* pClient, const char* pHasSendedData, const int nLen) {}

	virtual void didDumpText(const char* pText, bool isErrorDump=false) {}
};


//====================================================================================
//
//				CG2XSocketServer类定义
//
//====================================================================================


typedef struct _tagThreadParams_WORKER
{
	CG2XSocketServer* pIOCPModel;                             // (SocketServer)类指针，用于调用类中的函数
	int         nThreadNo;                                    // 线程编号
} THREADPARAMS_WORKER,*PTHREADPARAM_WORKER; 

// CG2XSocketServer类
class CG2XSocketServer
{
public:
	CG2XSocketServer(CG2XServerSocketDelegate* pDelegate);
	~CG2XSocketServer(void);

public:

	/**
	   启动服务器
      nPort: 监听的端口
	  nThreadCount: 工作者线程的个数, if nThreadCount <= 0 then thread count = number of cpu cores * 2
	  nAccecpExCount: 同时投递的AccecptEx请求个数
	*/
	bool Start(
		const short int nPort, 
		int nThreadCount = 0, 
		int nAccecptExCount = 10);

	//	停止服务器
	void Stop();

	// 加载Socket库
	bool Initialize();

	// 卸载Socket库，彻底完事
	void UnloadSocketLib() { WSACleanup(); }

	// 获得本机的IP地址
	char* GetLocalIP();

	// 设置监听端口
	void SetPort( const int& nPort ) { m_nPort=nPort; }

	int GetPort() { return m_nPort; }

	/* send data to client */
	int SendData(PER_SOCKET_CONTEXT* pClient, const void* pData, const int nDataLen);

protected:

	/* Initialize IOCP */
	bool _InitializeIOCP();

	/* Initialize Listenning Socket */
	bool _InitializeListenSocket();

	void _UnInitialize();

	// 投递Accept请求
	bool _PostAccept( PER_IO_CONTEXT* pAcceptIoContext ); 

	// 投递接收数据请求
	bool _PostRecv( PER_IO_CONTEXT* pIoContext );

	bool _PostSend( PER_IO_CONTEXT* pIoContext );

	// 处理客户端连入操作
	bool _DoAccpet( PER_SOCKET_CONTEXT* pSocketContext, PER_IO_CONTEXT* pIoContext );

	// 在有接收的数据到达的时候，进行处理
	bool _DoRecv( PER_SOCKET_CONTEXT* pSocketContext, PER_IO_CONTEXT* pIoContext );

	bool _DoSend( PER_SOCKET_CONTEXT* pSocketContect, PER_IO_CONTEXT* pIoContext );

	// 将客户端的相关信息存储到数组中
	void _AddToContextList( PER_SOCKET_CONTEXT *pSocketContext );

	// 将客户端的信息从数组中移除
	void _RemoveContext( PER_SOCKET_CONTEXT *pSocketContext );

	// 清空客户端信息
	void _ClearContextList();

	// 将句柄绑定到完成端口中
	bool _AssociateWithIOCP( PER_SOCKET_CONTEXT *pContext);

	// 处理完成端口上的错误
	bool HandleError( PER_SOCKET_CONTEXT *pContext,const DWORD& dwErr );

	// 线程函数，为IOCP请求服务的工作者线程
	static DWORD WINAPI _WorkerThread(LPVOID lpParam);

	// 获得本机的处理器数量
	int _GetNoOfProcessors();

	// 判断客户端Socket是否已经断开
	bool _IsSocketAlive(SOCKET s);


private:

	HANDLE                       m_hShutdownEvent;              // 用来通知线程系统退出的事件，为了能够更好的退出线程

	HANDLE                       m_hIOCompletionPort;           // 完成端口的句柄

	HANDLE*                      m_phWorkerThreads;             // 工作者线程的句柄指针

	int		                     m_nThreads;                    // 生成的线程数量

	char						 m_strIP[24];                   // 服务器端的IP地址
	int                          m_nPort;                       // 服务器端的监听端口

	CDialog*                     m_pMain;                       // 主界面的界面指针，用于在主界面中显示消息

	CRITICAL_SECTION             m_csContextList;               // 用于Worker线程同步的互斥量

	CRITICAL_SECTION			 m_csDelegate;					// 事件委托对象调用的互斥量

	CArray<PER_SOCKET_CONTEXT*>  m_arrayClientContext;          // 客户端Socket的Context信息        

	PER_SOCKET_CONTEXT*          m_pListenContext;              // 用于监听的Socket的Context信息

	LPFN_ACCEPTEX                m_lpfnAcceptEx;                // AcceptEx 和 GetAcceptExSockaddrs 的函数指针，用于调用这两个扩展函数

	LPFN_GETACCEPTEXSOCKADDRS    m_lpfnGetAcceptExSockAddrs; 

	CG2XServerSocketDelegate* m_pEventDelegate;
	
	int m_nAcceptExCount;
	

};

