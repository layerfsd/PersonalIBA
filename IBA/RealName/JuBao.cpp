#include "stdafx.h"
#include "JuBao.h"
#include <string>
#include "..\IBA_DEBUG.h"
#include "..\IBALog.h"
#include "..\IBA.h"

using namespace std;
CJubaoDelegate * CJuBao::m_pJubaoDelegate = NULL;
CJuBao::CJuBao()
{
	//加载动态库 
	m_hInst = LoadLibrary(_T("jubaonbimsinterface.dll")); 
	m_bInitSuccess = FALSE;
	if(!m_hInst) 
	{ 
		AfxMessageBox(_T("加载聚宝动态库失败"));
		IBA_LOG0(_T("加载聚宝动态库失败"));
	} 
}

int CJuBao::RegCallBackFunc()
{
	typedef int (__stdcall *Registercbinterface)
		(void(*)(char*,char*,DWORD,char*,char*));
	Registercbinterface funRegistercbinterface;

	if(!m_hInst)
		return -1;

	funRegistercbinterface = (Registercbinterface)GetProcAddress(m_hInst,"registercbinterface");
	
	if(!funRegistercbinterface)
	{
		IBA_LOG0(_T("读取聚宝动态库函数registercbinterface失败"));
		return -1;
	}

	int nRet = -1;
	if(funRegistercbinterface)
	{
		nRet = funRegistercbinterface(CheckEnd);		// 0成功
		IBA_LOG(_T("registercbinterface函数返回：%d"), nRet);
	}
	else
	{
		IBA_LOG0(_T("registercbinterface函数读取失败!"));
	}
	if(0 == nRet)
	{
		nRet = TRUE;
	}
	else
	{
		nRet = FALSE;
	}
	return nRet;
}

int CJuBao::InitInterface( std::string netbarid, std::string realid, std::string jubaosn )
{
	std::string regInfo="<?xml version=\"1.0\" \
						encoding=\"GB2312\"?><root><netbarid>"+netbarid+"</netbarid><realid>"+realid+"</\
						realid><jubaosn>"+jubaosn+"</jubaosn></root>"; 

	typedef    int (__stdcall *InitInterface)(char*);    
	InitInterface funInitInterface; 

	if(!m_hInst)
		return -1;

	funInitInterface = (InitInterface)GetProcAddress(m_hInst,"initinterface"); 
	
	if(!funInitInterface)
	{
		IBA_LOG0(_T("读取聚宝动态库函数initinterface失败"));
		return -1;
	}
	int nRet = -1;
	if(funInitInterface)
	{
		char szTemp[400]; 
		strcpy(szTemp, regInfo.c_str()); 
		nRet = funInitInterface(szTemp);			// 0成功	
		IBA_LOG(_T("initinterface函数返回：%d"), nRet);
	}
	else
	{
		IBA_LOG0(_T("initinterface函数读取失败!"));
	}
	if(0 == nRet)
	{
		m_bInitSuccess = TRUE;
		nRet = TRUE;
	}
	else
	{
		m_bInitSuccess = FALSE;
		nRet = FALSE;
	}
	return nRet;

	
}

std::string CJuBao::Decode( const char* Data,int DataByte,int& OutByte )
{ 
	//解码表 
	const char DecodeTable[] = 
	{ 
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
			62, // '+' 
			0, 0, 0, 
			63, // '/' 
			52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9' 
			0, 0, 0, 0, 0, 0, 0, 
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 
			13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z' 
			0, 0, 0, 0, 0, 0, 
			26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 
			39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z' 
	}; 
	//返回值 
	std::string strDecode; 
	int nValue; 
	int i= 0; 
	while (i < DataByte) 
	{ 
		if (*Data != '\r' && *Data!='\n') 
		{ 
			nValue = DecodeTable[*Data++] << 18; 
			nValue += DecodeTable[*Data++] << 12; 
			strDecode+=(nValue & 0x00FF0000) >> 16; 
			OutByte++; 
			if (*Data != '=') 
			{ 
				nValue += DecodeTable[*Data++] << 6; 
				strDecode+=(nValue & 0x0000FF00) >> 8; 
				OutByte++; 
				if (*Data != '=') 
				{ 
					nValue += DecodeTable[*Data++]; 
					strDecode+=nValue & 0x000000FF; 
					OutByte++; 
				} 
			} 
			i += 4; 
		} 
		else// 回车换行,跳过 
		{ 
			Data++; 
			i++; 
		} 
	} 
	return strDecode; 
}   

void CJuBao::CheckEnd( char* username, char* idcardno ,DWORD checkresult,char * transNumber,char * bitmap )
{ 
	if(m_pJubaoDelegate)
	{
		m_pJubaoDelegate->DoCheckEnd(username, idcardno, checkresult, transNumber, bitmap);
	}
}

int CJuBao::VerifyUserInfo( std::string name, std::string idCard, std::string stPhotoBuf )
{ 
	if(!m_bInitSuccess)
	{
		return -1;
	}

	std::string str="<root><username>"+name+"</username>" + 
		"<cardnumber>"+idCard+"</cardnumber>" + 
		"<usercameraimg>"+ stPhotoBuf+ "</usercameraimg>" 
		"</root>"; 
	typedef    int (__stdcall *CheckInterface)(char*);     
	CheckInterface funCheckInterface; 

	if(!m_hInst)
		return -1;

	funCheckInterface=(CheckInterface)GetProcAddress(m_hInst,"checkinterface"); 
	if(!funCheckInterface)
	{
		IBA_LOG0(_T("读取聚宝动态库函数checkinterface失败"));
		return -1;
	}

	int nRet = -1;
	if(funCheckInterface)
	{	
		char szTemp[1024*100]; 
		strcpy(szTemp, str.c_str()); 
		nRet =  funCheckInterface(szTemp); 
		// 0成功 1失败 2线程未终止
		IBA_LOG(_T("checkinterface函数返回：%d"), nRet);
	}
	else
	{
		IBA_LOG0(_T("checkinterface函数读取失败!"));
	}
	return nRet;
	
}

std::string CJuBao::Encode( const char* Data,int DataByte)
{
	//编码表
	const char EncodeTable[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	//返回值
	string strEncode;
	unsigned char Tmp[4]={0};
	int LineLength=0;
	for(int i=0;i<(int)(DataByte / 3);i++)
	{
		Tmp[1] = *Data++;
		Tmp[2] = *Data++;
		Tmp[3] = *Data++;
		strEncode+= EncodeTable[Tmp[1] >> 2];
		strEncode+= EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];
		strEncode+= EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];
		strEncode+= EncodeTable[Tmp[3] & 0x3F];
		if(LineLength+=4,LineLength==76) {strEncode+="\r\n";LineLength=0;}
	}
	//对剩余数据进行编码
	int Mod=DataByte % 3;
	if(Mod==1)
	{
		Tmp[1] = *Data++;
		strEncode+= EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode+= EncodeTable[((Tmp[1] & 0x03) << 4)];
		strEncode+= "==";
	}
	else if(Mod==2)
	{
		Tmp[1] = *Data++;
		Tmp[2] = *Data++;
		strEncode+= EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode+= EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];
		strEncode+= EncodeTable[((Tmp[2] & 0x0F) << 2)];
		strEncode+= "=";
	}

	return strEncode;
}



