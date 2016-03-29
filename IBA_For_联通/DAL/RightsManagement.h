#pragma once

// CRightsManagement 
//      权限管理
// Remark:
//      全局唯一对象, 收银台登录时创建权限管理
//
// CRightsManagement::HasRight :判断收银员是否具有某权限
// CRightsManagement::TakeRight :取得权限,如果收银员具有某权限，TakeRight返回TRUE
//      如果收银员没有权限，弹出管理员密码输入框，密码输入正确后，TakeRight返回TRUE。其他情况返回FALSE

namespace NS_DAL{

#define TAKING_RIGHTS_DEFINE(rightName) \
	BOOL CRightsManagement::Has_##rightName()\
	{\
	CString strName = _T(#rightName);\
	return CRightsManagement::HasRight(strName);}\
	BOOL Take_##rightName(BOOL bShouldOnDuty=TRUE)\
	{\
	CString strName = _T(#rightName);\
	return CRightsManagement::TakeRight(strName,bShouldOnDuty);	}\
	void SetUI_##rightName(CCmdUI* pCmdUI, BOOL bShouldOnDuty=TRUE)\
	{\
	CString strName = _T(#rightName);\
	CRightsManagement::SetCmdUI(pCmdUI, strName, bShouldOnDuty);}

class CRightsManagement
{
protected:
	CRightsManagement()
	{
		m_bIsBoss = FALSE;
	}

public:
	static CRightsManagement* GetInstance()
	{
		static CRightsManagement rm;
		return &rm;
	}

	void CreateManagement(LPCTSTR lpRightsList, BOOL bBoss);

	// 是否具有某权限
	BOOL HasRight(LPCTSTR lpRightName);

	// 取得权限，没有权限时弹出管理员密码输入框
	BOOL TakeRight(LPCTSTR lpRightName, BOOL bShouldOnDuty=TRUE);

	void SetCmdUI(CCmdUI *pCmdUI, LPCTSTR lpszName, BOOL bShouldOnDuty=TRUE);
	
public:
	//----------------------------------------//
	//                  权限定义                 //
	//----------------------------------------//

	// 收银台设置
	TAKING_RIGHTS_DEFINE(IBAP_CONFIG);

	// 闲时包月
	TAKING_RIGHTS_DEFINE(IBAP_BUNDLETIME2);

	// 合并账户
	TAKING_RIGHTS_DEFINE(IBAP_MERGEMEMBER);

	// 积分兑换商品
	TAKING_RIGHTS_DEFINE(IBAP_USEPOINT);

	// 商品销售
	TAKING_RIGHTS_DEFINE(IBAP_RETAIL);

	// 收银员操作查询
	TAKING_RIGHTS_DEFINE(IBAP_OPERATIONQUERY);

	// 收银员交款查询
	TAKING_RIGHTS_DEFINE(IBAP_CASHSHIFTQUERY);

	// 终端消费记录
	TAKING_RIGHTS_DEFINE(IBAP_TERMCONSUME);

	// 查看消费记录
	TAKING_RIGHTS_DEFINE(IBAP_CONSUMELIST);

	// 充值
	TAKING_RIGHTS_DEFINE(IBAP_CREDIT);

	// 充值卡充值
	TAKING_RIGHTS_DEFINE(IBAP_CARDCREDIT);

	// 用户升级
	TAKING_RIGHTS_DEFINE(IBAP_UPGRADE);

	// 修改账号
	TAKING_RIGHTS_DEFINE(IBAP_CHANGESN);

	// 包时
	TAKING_RIGHTS_DEFINE(IBAP_BUNDLETIME);

	// 修改用户资料
	TAKING_RIGHTS_DEFINE(IBAP_MODIFYUSER);

	// 账户直充
	TAKING_RIGHTS_DEFINE(IBAP_CREDITDIRECT);

	// 修改密码
	TAKING_RIGHTS_DEFINE(IBAP_MODIFYPWD);

	// 网吧参数设置
	TAKING_RIGHTS_DEFINE(IBAP_NETBARSET);

	// 上下班权限
	TAKING_RIGHTS_DEFINE(IBAP_ONOFFDUTY);

	// 开户权限
	TAKING_RIGHTS_DEFINE(IBAP_REGISTER);

	// 商品退货权限
	TAKING_RIGHTS_DEFINE(IBAP_RETAILRETURN);

	// 赠送商品权限
	TAKING_RIGHTS_DEFINE(IBAP_RETAILPRESENT);

	// 积分兑换商品
	TAKING_RIGHTS_DEFINE(IBAP_RETAILPOINT);

	// 充值修正权限
	TAKING_RIGHTS_DEFINE(IBAP_CREDITREVERSE);

private:
	CString m_strRightsList;
	BOOL m_bIsBoss;
};

}