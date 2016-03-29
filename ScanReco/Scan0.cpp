// Scan0.cpp: implementation of the CScan0 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Scan0.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScan0::CScan0()
{

}

CScan0::~CScan0()
{

}

bool CScan0::IsDeviceValid()
{
	return false;
}

int CScan0::ReadCard(IDENTITY_INFO& /*info*/, char* /*szHeadImage*/)
{
	return ERROR_SCANNER_UNSUPPORT;
}

void CScan0::GetErrorMessage(int /*errCode*/,char* /*szErrMsg*/)
{
}

int CScan0::OpenDevice()
{
	return ERROR_SCANNER_UNSUPPORT;
}

int CScan0::CloseDevice()
{
	return ERROR_SCANNER_UNSUPPORT;
}

int CScan0::ScanImage(int /*idType*/, char* /*szImgFile*/)
{
	return ERROR_SCANNER_UNSUPPORT;
}

int CScan0::RecogniseImage(int /*idType*/, IDENTITY_INFO& /*info*/, char* /*szHeadImage*/)
{
	return ERROR_SCANNER_UNSUPPORT;
}

bool CScan0::IsSupportHeadImage(int /*idType*/)
{
	return false;
}

bool CScan0::IsSupportRecognise(int /*idType*/)
{
	return false;
}

bool CScan0::IsSupportCallBack()
{
	return false;
}

bool CScan0::IsSupportScanner()
{
	return false;
}

int CScan0::SetCallBack(CallBack /*pFunc*/)
{
	return ERROR_SCANNER_UNSUPPORT;
}

bool CScan0::IsSupportReader()
{
	return false;
}

