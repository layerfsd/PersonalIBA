// HX_FDX3CardReader.cpp: implementation of the CHX_FDX3CardReader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HX_FDX3CardReader.h"


const char *g_cNation[][2] = 
{
"01", "汉",
"02", "满",
"03", "侗",
"04", "瑶",
"05", "白",
"06", "土家",
"07", "哈尼",
"08", "哈萨克",
"09", "傣",
"10", "黎",
"11", "傈僳",
"12", "佤",
"13", "畲",
"14", "高山",
"15", "拉祜",
"16", "水",
"17", "东乡",
"18", "纳西",
"19", "景颇",
"20", "柯尔克孜",
"21", "土",
"22", "达斡尔",
"23", "仫佬",
"24", "羌",
"25", "布朗",
"26", "撒拉",
"27", "毛难",
"28", "仡佬",
"29", "锡伯",
"30", "阿昌",
"31", "普米",
"32", "塔吉克",
"33", "怒",
"34", "乌孜别克",
"35", "俄罗斯",
"36", "鄂温克",
"37", "崩龙",
"38", "保安",
"39", "裕固",
"40", "京",
"41", "塔塔尔",
"42", "独龙",
"43", "鄂伦春",
"44", "赫哲",
"45", "门巴",
"46", "珞巴",
"47", "基诺",
"48", "蒙古",
"49", "回",
"50", "藏",
"51", "维吾尔",
"52", "苗",
"53", "彝",
"54", "壮",
"55", "布依",
"56", "朝鲜",
"57", "其他",
"58", "外国血统",
0, 0,
};
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHX_FDX3CardReader::CHX_FDX3CardReader()
{
	m_hDll = NULL;
	m_bIsOpen = FALSE;

	lpfnOpenPort = NULL;
	lpfnClosePort = NULL;
	lpfnStartFindIDCard = NULL;
	lpfnSelectIDCard = NULL;
	lpfnReadBaseMsg = NULL;
	lpfnGetBmp = NULL;

}

CHX_FDX3CardReader::~CHX_FDX3CardReader()
{
	Close();
}

BOOL CHX_FDX3CardReader::OpenPort(int iPort)
{
	if(m_bIsOpen)
	{
		return FALSE;
	}

	m_hDll = LoadLibrary("SDTAPI.DLL");
	if(m_hDll == NULL)
	{
		return FALSE;
	}


	
	lpfnOpenPort = (FUN_SDT_OpenPort *)GetProcAddress(m_hDll, "SDT_OpenPort");
	lpfnClosePort = (FUN_SDT_ClosePort *)GetProcAddress(m_hDll, "SDT_ClosePort");
	lpfnStartFindIDCard = (FUN_SDT_StartFindIDCard  *)GetProcAddress(m_hDll, "SDT_StartFindIDCard");
	lpfnSelectIDCard = (FUN_SDT_SelectIDCard *)GetProcAddress(m_hDll, "SDT_SelectIDCard");
	lpfnReadBaseMsg = (FUN_SDT_ReadBaseMsg *)GetProcAddress(m_hDll, "SDT_ReadBaseMsg");

	if(	lpfnOpenPort == NULL ||
		lpfnClosePort == NULL ||
		lpfnStartFindIDCard == NULL ||
		lpfnSelectIDCard == NULL ||
		lpfnReadBaseMsg == NULL )
	{
		FreeLibrary(m_hDll);

		return FALSE;
	}

	m_bIsOpen = TRUE;

	if(lpfnOpenPort(iPort) != 0x90)
	{
		Close();
		m_bIsOpen = FALSE;

	}

	return TRUE;
}

int CHX_FDX3CardReader::ClosePort(int iPort)
{
	if(!m_bIsOpen)
	{
		return 0x01;
	}

	int iError = lpfnClosePort(iPort);
	if(iError != 0x90)
	{
	}
	Close();

	return iError;
}

void CHX_FDX3CardReader::Close()
{
	if(m_bIsOpen)
	{
		if(m_hDll != NULL)
		{
			FreeLibrary(m_hDll);
			
			m_hDll = NULL;
		}
		m_bIsOpen = FALSE;
		
		lpfnOpenPort = NULL;
		lpfnClosePort = NULL;
		lpfnStartFindIDCard = NULL;
		lpfnSelectIDCard = NULL;
		lpfnReadBaseMsg = NULL;
	}

}

int CHX_FDX3CardReader::StartFindIDCard ( int iPort , unsigned char * pucManaInfo, int iIfOpen )
{
	if(!m_bIsOpen)
	{
		return 0x01;
	}

	return lpfnStartFindIDCard(iPort, pucManaInfo, iIfOpen );
}

int CHX_FDX3CardReader::SelectIDCard ( int iPort , unsigned char * pucManaMsg, int iIfOpen )
{
	if(!m_bIsOpen)
	{
		return 0x01;
	}

	return lpfnSelectIDCard(iPort, pucManaMsg, iIfOpen );
}

int CHX_FDX3CardReader::ReadBaseMsg ( int iPort, unsigned char * pucCHMsg, unsigned int * puiCHMsgLen, unsigned char * pucPHMsg, unsigned int * puiPHMsgLen, int iIfOpen)
{
	if(!m_bIsOpen)
	{
		return 0x01;
	}

	return lpfnReadBaseMsg(iPort, pucCHMsg, puiCHMsgLen, pucPHMsg, puiPHMsgLen, iIfOpen);
}


char * CHX_FDX3CardReader::GetNationStr(char *pCode)
{
	if(IsBadReadPtr(pCode, 2) || strlen(pCode) < 2)
		return "其他";

	for(int i = 0; g_cNation[i][0] != 0; i++)
	{
		if(strncmp(g_cNation[i][0], pCode, 2) == 0)
		{
			return (char*)g_cNation[i][1];
		}
	}

	return "其他";
}

int CHX_FDX3CardReader::GetBmp(char *Wlt_File, int intf)
{
	if(IsBadCodePtr((FARPROC)lpfnGetBmp))
	{
		HINSTANCE hDll = LoadLibrary("WltRS.dll");
		if(hDll != NULL)
		{
			lpfnGetBmp = (FUN_GetBmp*)GetProcAddress(hDll, "GetBmp");
			if(!IsBadCodePtr((FARPROC)lpfnGetBmp))
			{
				return lpfnGetBmp(Wlt_File, intf);
			}
		}
	}
	else
	{
		return lpfnGetBmp(Wlt_File, intf);
	}

	return -5;
}
