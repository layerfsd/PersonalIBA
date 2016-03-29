// HX_FDX3CardReader.h: interface for the CHX_FDX3CardReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HX_FDX3CARDREADER_H__F1179ED9_071F_44AB_8466_61E7AF269277__INCLUDED_)
#define AFX_HX_FDX3CARDREADER_H__F1179ED9_071F_44AB_8466_61E7AF269277__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


 
//90 	操作成功 	   
//91 	证/卡中此项无内容 	   
//9F 	寻找找证/卡成功 	   
//01 	端口打开失败/端口尚未打开/端口号不合法 	   
//02 	PC接收超时，在规定的时间内未接收到规定长度的数据。 	   
//03 	数据传输错误 	   
//05 	该SAM_V串口不可用，只在SDT_GetCOMBaud时才有可能返回 	   
//10 	接收业务终端数据的校验和错 	   
//11 	接收业务终端数据的长度错。 	   
//21 	接收业务终端的命令错误，包括命令中的各种数值或逻辑搭配错误 	   
//23 	越权操作 	   
//24 	无法识别的错误 	   
//80 	寻找证/卡失败 	   
//81 	选取证/卡失败 	 


//31 	证/卡证认SAM_V失败 	   
//32 	SAM_V认证证/卡失败 	   
//33 	信息验证错误 	   
//40 	无法识别的卡类型 	   
//41 	读证/卡操作失败 	   
//47 	取随机数失败 	   
//60 	SAM_V自检失败，不能接收命令 	   
//66 	SAM_V没经过授权,无法使用 	 



typedef int (WINAPI FUN_SDT_OpenPort)( int iPort );
typedef int (WINAPI FUN_SDT_ClosePort)( int iPort );
typedef int (WINAPI FUN_SDT_StartFindIDCard )( int iPort , unsigned char * pucManaInfo, int iIfOpen );
typedef int (WINAPI FUN_SDT_SelectIDCard )( int iPort , unsigned char * pucManaMsg, int iIfOpen );
typedef int (WINAPI FUN_SDT_ReadBaseMsg )( int iPort, unsigned char * pucCHMsg, unsigned int * puiCHMsgLen, unsigned char * pucPHMsg, unsigned int * puiPHMsgLen, int iIfOpen);

typedef int (PASCAL FUN_GetBmp)(char * Wlt_File, int intf);




class CHX_FDX3CardReader  
{
public:
	void Close();
	BOOL OpenPort(int iPort = 1001);
	int ClosePort( int iPort = 1001);
	int StartFindIDCard ( int iPort , unsigned char * pucManaInfo, int iIfOpen );
	int SelectIDCard ( int iPort , unsigned char * pucManaMsg, int iIfOpen );
	int ReadBaseMsg ( int iPort, unsigned char * pucCHMsg, unsigned int * puiCHMsgLen, unsigned char * pucPHMsg, unsigned int * puiPHMsgLen, int iIfOpen);
	int GetBmp(char * Wlt_File, int intf = 2);
	char * GetNationStr(char *pCode = "01");

	
	CHX_FDX3CardReader();
	virtual ~CHX_FDX3CardReader();

private:
	BOOL m_bIsOpen;
	HINSTANCE m_hDll;
	
	FUN_SDT_OpenPort *lpfnOpenPort;
	FUN_SDT_ClosePort *lpfnClosePort;
	FUN_SDT_StartFindIDCard  *lpfnStartFindIDCard;
	FUN_SDT_SelectIDCard *lpfnSelectIDCard;
	FUN_SDT_ReadBaseMsg *lpfnReadBaseMsg;
	FUN_GetBmp *lpfnGetBmp;

public:
	typedef struct tag_TextInfo
	{
		//unsigned char ucUndefine[14];
		unsigned char ucName[30];
		unsigned char ucSex[2];
		unsigned char ucNation[4];
		unsigned char ucBirthDay[16];
		unsigned char ucAddress[70];
		unsigned char ucID[36];
		unsigned char ucIssue[30];
		unsigned char ucExpirStart[16];
		unsigned char ucExpirEnd[16];

	}TAG_TEXT_INFO;

};

#endif // !defined(AFX_HX_FDX3CARDREADER_H__F1179ED9_071F_44AB_8466_61E7AF269277__INCLUDED_)
