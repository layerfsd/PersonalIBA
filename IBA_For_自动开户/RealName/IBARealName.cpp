
#include "stdafx.h"
#include "RZXRealName.h"
#include "GLWRealName.h"
#include "WDRealName.h"
#include "QDHXRealName.h"
#include "YBWWRealName.h"
#include "..\IBA.h"
#include ".\ibarealname.h"
#include "..\Socket\LocalServer.h"

using namespace NS_REALNAME;

CIBARealName::CIBARealName(void)
: m_lpRealName(NULL)
{
	CreateRealName();
}

CIBARealName::~CIBARealName(void)
{
	DestoryRealName();
}

BOOL CIBARealName::CreateRealName()
{
	//0:无实名 1:任子行 2:过滤王 3:摩根 4:桑地 5:网盾 6.任子行2008

#ifdef _DEBUG

	//m_lpRealName = new CRZXRealName;

	//m_lpRealName = new CQDHXRealName;

	//m_lpRealName = new CGLWRealName;

	//m_lpRealName = new CWDRealName;

	//m_lpRealName = new CYBWWRealName;

	//m_lpRealName = new CNoneRealName;

	switch (CNetBarConfig::GetInstance()->GetAuditType())
	{
	case 0:
		m_lpRealName = new CNoneRealName;
		break;

	case 1:
	case 6:
		m_lpRealName = new CRZXRealName;
		break;

	case 2:
		m_lpRealName = new CGLWRealName;
		break;

	case 3:
		m_lpRealName = new CYBWWRealName;
		break;

	case 5:
		m_lpRealName = new CWDRealName;
		break;

	case 9:
		m_lpRealName = new CQDHXRealName;
		break;
	}

#else
	
	int nType = CNetBarConfig::GetInstance()->GetAuditType();

	switch (CNetBarConfig::GetInstance()->GetAuditType())
	{
	case 0:
		m_lpRealName = new CNoneRealName;
		break;

	case CIBAGlobal::emRealnameRZX:
	case CIBAGlobal::emRealnameRZX2008:
		m_lpRealName = new CRZXRealName;
		break;

	case 2:
		m_lpRealName = new CGLWRealName;
		break;

	case 3:
		m_lpRealName = new CYBWWRealName;
		break;

	case 5:
		m_lpRealName = new CWDRealName;
		break;

	case 9:
		m_lpRealName = new CQDHXRealName;
		break;
	}

#endif

	return (m_lpRealName != NULL);
}

void CIBARealName::DestoryRealName()
{
	if (m_lpRealName != NULL)
	{
		delete m_lpRealName;
		m_lpRealName = NULL;
	}
}

INT CIBARealName::OnRealNameMsg(UINT nMsgType, LPVOID lpData /*= NULL*/, DWORD dwLen /*= 0*/)
{
	IBA_TRACE("CIBARealName::OnRealNameMsg...");

	if (m_lpRealName != NULL)
	{
		return m_lpRealName->OnRealNameMsg(nMsgType, lpData, dwLen);
	}

	return 0;
}

void CIBARealName::OnCardIn()
{
	if (m_lpRealName != NULL)
	{
		m_lpRealName->OnCardIn();
	}
}

INT CIBARealName::InvokeRealName(UINT nMsgType, LPVOID lpData /*= NULL*/, DWORD dwLen /*= 0*/)
{
	if (m_lpRealName != NULL)
	{
		return m_lpRealName->InvokeRealName(nMsgType, lpData, dwLen);
	}

	return 0;
}

BOOL CIBARealName::InitRealName()
{	
	if (m_lpRealName != NULL)
	{		
		return m_lpRealName->InitRealName();
	}

	return TRUE;
	return FALSE;
}

BOOL CIBARealName::UnInitRealName()
{	
	if (m_lpRealName != NULL)
	{
		return m_lpRealName->UnInitRealName();
	}

	return FALSE;
}

BOOL CIBARealName::CheckRealName()
{
	if (m_lpRealName != NULL)
	{
		return m_lpRealName->CheckRealName();
	}

	return TRUE;
}

BOOL CIBARealName::ScanUserInfo(UINT nType, CNetBarUser& NetBarUser, UINT nOption)
{
	if (m_lpRealName != NULL)
	{
		return m_lpRealName->ScanUserInfo(nType, NetBarUser, nOption);
	}

	return FALSE;
}

BOOL CIBARealName::DoScan(UINT nType)
{
	if (m_lpRealName != NULL)
	{
		return m_lpRealName->DoScan(nType);
	}

	return FALSE;
}

BOOL CIBARealName::IsScanEnabled()
{
	if (m_lpRealName != NULL)
	{
		return m_lpRealName->IsScanEnabled();
	}

	return FALSE;
}

BOOL CIBARealName::UploadImage(UINT nType, CNetBarUser& NetBarUser, UINT nOption)
{
	if (m_lpRealName != NULL)
	{
		return m_lpRealName->UploadImage(nType, NetBarUser, nOption);
	}
	
	return FALSE;
}

BOOL CIBARealName::ActivationMember(const CNetBarUser& netBarUser)
{
	if (m_lpRealName != NULL)
	{
		return m_lpRealName->ActivationMember(netBarUser);
	}

	return FALSE;
}

BOOL CIBARealName::IsRegisterEnabled()
{
	if (m_lpRealName != NULL)
	{
		return m_lpRealName->IsRegisterEnabled();
	}

	return FALSE;
}

BOOL CIBARealName::IsCheckInEnabled()
{
	if (m_lpRealName != NULL)
	{
		return m_lpRealName->IsCheckInEnabled();
	}

	return FALSE;
}

BOOL CIBARealName::IsMenuEnabled()
{
	if (m_lpRealName != NULL)
	{
		return m_lpRealName->IsMenuEnabled();
	}

	return FALSE;
}

INT CIBARealName::OnExecuteMsg(UINT nExecuteType, LPVOID lpData /*= NULL*/, DWORD dwLen /*= 0*/)
{
	if (m_lpRealName != NULL)
	{
		return m_lpRealName->OnExecuteMsg(nExecuteType, lpData, dwLen);
	}

	return 0;
}

BOOL CIBARealName::NeedToCheckRegisterData()
{
	if (m_lpRealName != NULL)
	{
		return m_lpRealName->NeedToCheckRegisterData();
	}

	return FALSE;
}

BOOL CIBARealName::IsRealNameExtendMenuEnabled()
{
	if (m_lpRealName != NULL)
	{
		return m_lpRealName->IsRealNameExtendMenuEnabled();
	}

	return FALSE;
}

BOOL CIBARealName::RobFocusAfterSwipingCard()
{
	if (m_lpRealName != NULL)
	{
		return m_lpRealName->RobFocusAfterSwipingCard();
	}

	return FALSE;
}

void CIBARealName::OnCashierCheckIn()
{
	if (m_lpRealName != NULL)
	{
		m_lpRealName->OnCashierCheckIn();
	}
}

void CIBARealName::AuditUserCheckOut(CString strTermID,CString strIP,UINT nMemberID)
{
	CLocalServer::GetInstance()->LocalAuditUserQuit(strTermID, strIP, nMemberID);
}