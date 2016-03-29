#pragma once
#include "g2xcontrol.h"

G2XUI_BEGIN

class G2X_CLASSES CG2XPictureCtrl : public CG2XControl
{
	DECLARE_DYNCREATE(CG2XPictureCtrl)
	DECLARE_GDI_MESSAGE_MAP()
public:
	CG2XPictureCtrl(void);
	virtual ~CG2XPictureCtrl(void);

protected:
	virtual void DrawControl(Graphics* pGraphics,G2XControlState ctrlState);

public:
	void SetStretch(BOOL bStretch);
	void SetImage(Image* pImg, BYTE nAlpha=255, Color* lpTransparentClr=NULL);
	void SetImage(UINT nResID, BYTE nAlpha=255, Color* lpTransparentClr=NULL);
	Bitmap* GetImage();

	Bitmap* m_pImage;
	BYTE m_nAlpha;
	BOOL m_bStretch;
};

G2X_END
