#pragma once

#include "RespField.h"

using namespace NS_SERVLET::NS_SERVLET_HELPPER;

class CCashier;
class CMainFrame;
class CLocalCashier;
class CThreadHelpper;

namespace NS_SERVLET
{

enum ECENTER
{
	E_LOCAL_CENTER = 0,     //本地中心
	E_MAIN_CENTER = 1,      //主中心
	E_BACKUP_CENTER = 2,    //备份中心
	E_OTHER_CENTER = 3,     //其他中心(异地中心)
	E_QUERY_CENTER = 4,     //主查询机
	E_QUERY_CENTER2 = 5     //备查询机
};

enum ECARDTYPE
{
	ECT_IC,
	ECT_ID
};

enum ESTATUSCODE
{
	ESC_WXYH1        = 1,  //!<1无效用户
	ESC_WXWB2        = 2,  //!<2无效网吧
	ESC_QXDCYH3      = 3,  //!<3请先登出用户
	ESC_WXBS4        = 4,  //!<4无效包时
	ESC_GKYGS5       = 5,  //!<5该卡已挂失
	ESC_ZHYEBZ6      = 6,  //!<6账户余额不足
	ESC_LSHC7        = 7,  //!<7流水号不匹配
	ESC_WUYHJB8      = 8,  //!<8无效用户级别
	ESC_YJBYZ9       = 9,  //!<9押金不一致
	ESC_BFHZBS10     = 10, //!<10不符合转包时
	ESC_WSJJH11      = 11, //!<11未进行上机激活
	ESC_YHBZX12      = 12, //!<12用户不在线
	ESC_ZJHC13       = 13, //!<13证件号码不匹配
	ESC_WQXF14       = 14, //!<14无权消费
	ESC_WXMM15       = 15, //!<15无效密码
	ESC_CKHYBSY16    = 16, //!<16此卡号已被使用
	ESC_KNXXBYZ17    = 17, //!<17卡内信息不一致
	ESC_KMYBQP18     = 18, //!<18卡密钥不匹配
	ESC_YHYZX19      = 19, //!<19用户已注销
	ESC_JEWX20       = 20, //!<20金额无效
	ESC_YHFSTZ21     = 21, //!<21用户发生透支
	ESC_YHMYGS22     = 22, //!<22用户没有挂失
	ESC_FWQCSC23     = 23, //!<23服务器参数错
	ESC_KKJEBZ24     = 24, //!<24开卡金额不足
	ESC_GKBSDJK25    = 25, //!<25该卡不是冻结卡
	ESC_WXBFQQ26     = 26, //!<26无效并发请求
	ESC_CGCZYXSJ27   = 27, //!<27超过冲正允许时间
	ESC_FXWBSX28     = 28, //!<28发行网吧无效
	ESC_WXZDH29      = 29, //!<29无效终端号
	ESC_MACJYC30     = 30, //!<30MAC校验错
	ESC_YHYDC31      = 31, //!<31用户已登出
	ESC_QGHDYKJB32   = 32, //!<32请更换对应级别的卡
	ESC_GYHJBBYXTK33 = 33, //!<33该用户级别不允许退款
	ESC_WXKBCH34     = 34, //!<34无效卡批次号
	ESC_WXCZKLB35    = 35, //!<35无效充值卡类别
	ESC_JQQYBYZ36    = 36, //!<36机器区域不一致
	ESC_WJHHYCZ37    = 37, //!<37未激活或已充值
	ESC_CZKGQ38      = 38, //!<38充值卡已过期
	ESC_GKYDJ39      = 39, //!<39该卡已冻结
	ESC_CKWZC40      = 40, //!<40此卡未注册
	ESC_KHBCZ41      = 41, //!<41卡号不存在
	ESC_WXLSLX42     = 42, //!<42无效零售类型
	ESC_WXZFLX43     = 43, //!<43无效支付类型
	ESC_CKBCZ44      = 44, //!<44仓库不存在
	ESC_WXZCID45     = 45, //!<45无效资产ID
	ESC_WQXF46       = 46, //!<46无权消费
	ESC_KHCW47       = 47, //!<47卡号串位
	ESC_YSHCW48      = 48, //!<48印刷号串位
	ESC_YHHCW49      = 49, //!<49用户号串位
	ESC_WXSYTID50    = 50, //!<50无效收银台ID
	ESC_WXDKQ51      = 51, //!<51无效读卡器
	ESC_WXGJSJ54     = 54, //!<54无效挂机时间
	ESC_GJWJS55      = 55, //!<55挂机未结束
	ESC_WBWBCSJ56    = 56, //!<56网吧未保存数据
	ESC_DQSDBYX57    = 57, //!<57当前时段不允许
	ESC_BSYJS58      = 58, //!<58包时已结束
	ESC_BSQYSX59     = 59, //!<59包时区域受限
	ESC_YJBS60       = 60, //!<60您已经包时
	ESC_WXYWLX61     = 61, //!<61无效业务类型
	ESC_WXZHLX62     = 62, //!<62无效账户类型
	ESC_YHXXWX63     = 63, //!<63用户信息无效
	ESC_YJYXXBCZ64   = 64, //!<64原交易信息不存在
	ESC_XMBNWYW65    = 65, //!<65姓名不能为英文字符
	ESC_ZJHMCF66     = 66, //!<66证件号码重复
	ESC_KKSXCX67     = 67, //!<67开卡数量超限
	ESC_YHBSSM68     = 68, //!<68用户不是实名
	ESC_SJQYSX69     = 69, //!<69上机区域受限
	ESC_WBDSMK70     = 70, //!<70未绑定实名卡
	ESC_BSRSYM71     = 71, //!<71包时人数已满
	ESC_YJSB72       = 72, //!<72已经上班
	ESC_SWSB73       = 73, //!<73尚未上班
	ESC_BCCS74       = 74, //!<74班次超时
	ESC_YHHKYZF75    = 75, //!<75用户或卡已作废
	ESC_QXFBSXX76    = 76, //!<76请修复包时信息
	ESC_QSHZC77      = 77, //!<77请稍候再试
	ESC_JZKK78       = 78, //!<78禁止开卡
	ESC_SMXLHWX79    = 79, //!<79实名序列号无效
	ESC_SMSZBQP80    = 80, //!<80实名设置不匹配
	ESC_SMXLHYCZ81   = 81, //!<81实名序列号已存在
	ESC_SHYZX82      = 82, //!<82实名已在线
	ESC_ZJLXBYX83    = 83, //!<83证件类型不允许
	ESC_JYSJGQ84     = 84, //!<84交易时间过期
	ESC_ZXZDCF85     = 85, //!<85在线终端重复
	ESC_CZJETD86     = 86, //!<86充值金额太低
	ESC_CKJZSY87     = 87, //!<87此卡禁止使用
	ESC_SYTYDQ88     = 88, //!<88收银台已到期
	ESC_ZZYEBZ89     = 89, //!<89充值额度不足
	ESC_DHKFCC90     = 90, //!<90电话扣费出错
	ESC_GNBZC91      = 91, //!<91功能不支持
	ESC_SYTBBD92     = 92, //!<92收银台版本太低
	ESC_JLZHBCZ96    = 96, //!<96奖励账户不存在
	ESC_WXBW97       = 97, //!<97无效报文
	ESC_SJKCW98      = 98, //!<98数据库错误
	ESC_QQSB99       = 99, //!<99请求失败
	ESC_QT100        = 100,//!<100其它
	ESC_QT101        = 101 //!<101其它1
};

class CServletMethod
{
protected :
	CServletMethod(LPCTSTR lpszMethodName);
	virtual ~CServletMethod(void);

public :

	friend class CCashier;
	friend class CLocalCashier;
	friend class CThreadHelpper;

	INT GetStatusCode();				//!< 中心返回的状态码

	CString GetStatusMessage();			//!< 中心返回的状态码对应的字符串信息

	void SetPSK(CString newVal);		//!< 设置加密密钥
	
	CString GetPSK() const;				//!< 读取加密密钥

	ECARDTYPE GetCardType() const;		//!< 读取当前卡类型

	void SetCardType(ECARDTYPE newVal); //!< 设置卡类型

	UINT GetSerialNo();					//!< 读取中心返回的流水号

	UINT GetLocalSerialNo() const;		//!< 读取本地操作的本地流水号

	CString GetUrlFromIndex(UINT nIndex) ;
protected :

	BOOL Execute();						//!< 执行请求
	void AddParams();					//!< 添加参数，子类可覆盖
	void AddRespFields();				//!< 添加返回字段，子类可覆盖
	CString MakeParamsString();			//!< 拼接参数并加密
	void WriteLog(LPCTSTR lpszInfo);	//!< 写日志文件

protected :
	
	virtual CString GetMethodName();					//!< 返回当前函数名称，可重写
	virtual CString GetRequestAddr(UINT nIndex);		//!< 获取中心地址，可重写
	virtual CString MergeParams();						//!< 合并参数，可重写
	virtual CString EncodeParams(CString strParams);	//!< 加密参数，可重写
	virtual UINT	MakeLocalSerialNo();				//!< 获取本地流水号
	virtual CString MakeMacString(CString strParams);	//!< 生成参数MAC，可重写
	virtual BOOL	VerifyResp(CString strResp);		//!< 校验返回加密数据，可重写
	virtual BOOL    DecodeResp(CString strResp);		//!< 解析返回值，可重写
	virtual BOOL	GetHttpData(LPCTSTR lpszURL, CString& strResp);	//!< 和中心交互，可重写
	virtual BOOL	GetCenterData(CString strParams, CString& strResp);
	virtual BOOL	OnBegin();							//!< 执行请求前
	virtual void	OnEnd(BOOL bRet);					//!< 执行请求后

protected :
	
	CInputParamList m_Params;		//!< 参数列表
    CRespFieldList  m_RespFields;	//!< 返回值列表

	const CString m_strMethodName;	//!< 方法名称
	CString m_strPSK;				//!< 密钥
	
	ECARDTYPE m_nCardType;			//!< 卡类型

	UINT m_nLocalSerialNo;			//!< 本地流水号

protected:

	UINT m_nNetbarId;				//!< 网吧ID

	BOOL m_bUseQueryHost;			//!< 使用查询机

	UINT m_nRevTimeout;				//!< 接收超时(单位:秒)

	static UINT m_nFirstCenterIndex;		//!< 首先连接的中心

	static UINT m_nFirstQueryCenterIndex;	//!< 首先连接的查询中心

	friend CMainFrame;
};

}