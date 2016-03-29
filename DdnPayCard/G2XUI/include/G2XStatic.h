#pragma once

G2XUI_BEGIN

class G2X_CLASSES CG2XStatic : public CG2XControl
{
	DECLARE_DYNCREATE(CG2XStatic)
	DECLARE_GDI_MESSAGE_MAP()
public:
	CG2XStatic(void);
	virtual ~CG2XStatic(void);

	virtual void DrawControl(Graphics* pGraphics,G2XControlState ctrlState);

protected:
	g_msg void OnCreate();

public:
	BOOL m_bAutoAdaptation; // 控件是否自适应文本的大小
};

G2X_END
