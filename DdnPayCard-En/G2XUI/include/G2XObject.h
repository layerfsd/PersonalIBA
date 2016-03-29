#pragma once
#include "G2XUser.h"

G2XUI_BEGIN

//------------------------------------------------------------------------
// class G2X_CLASSES CG2XObject 
//------------------------------------------------------------------------

struct GDI_MSGMAP;
typedef long HGDIWND;

class G2X_CLASSES CG2XObject : public CObject
{
	DECLARE_DYNCREATE(CG2XObject)

public:
	CG2XObject();
	virtual ~CG2XObject();

public:
	virtual BOOL OnGdiWndMsg(UINT nGMsgType, UINT nID, WPARAM wParam, LPARAM lParam, LRESULT* lpResult);

protected:
	static const GDI_MSGMAP msgMap;

public:
	HGDIWND m_hG2XWnd;
};


G2X_END

