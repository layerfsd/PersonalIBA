/////////////内部使用////////////////
#define WM_IREADER_MESSAGE	WM_USER+200
#define WM_IREADER_CARDIN	WM_IREADER_MESSAGE+1
#define WM_IREADER_CARDOUT	WM_IREADER_MESSAGE+2

#define CMD_READER_MSG					0xfe
#define CMD_READER_MSG_CARDIN			0x01
#define CMD_READER_MSG_CARDOUT			0x02
/////////////////////////////////////

#define OPT_WRITE_CARDINFO		0x00000001			//写卡信息[印刷号|卡号],会员信息[会员号|开卡网吧|会员级别], 参数段一
#define OPT_WRITE_TEMP			0x00000002			//写卡临时信息,参数段二
#define OPT_WRITE_BUNDLE		0x00000004			//写卡包时信息,参数段三
#define OPT_WRITE_CLEAR_TEMP	0x00000008			//清空临时信息
#define OPT_WRITE_CLEAR_BUNDLE	0x00000010			//清空包时信息



/////////////////////////////////////
#define CUSTOM_DATA_ADDR		0xb8	//len 18
#define MSG_CARD_CHECK_ERR		0x0D	//验证密码错误
//////////////4.9之无用//////////////
#define	MSG_EEPROM_ERR			0x04	//EEPROM 错误
#define	MSG_EEPROM_WR_ERR		0x05	//EEPROM 写错误
#define	MSG_EEPROM_RD_ERR		0x06	//EEPROM 读错误
#define	MSG_EEPROM_CHECKSUM		0x07	//EEPROM 校验合错误
////////////////////////////////////

#define	MSG_OK					0x01
#define	MSG_ERR					0x02
#define	MSG_CHECKSUM			0x03	//通讯中的数据校验合错误
#define	MSG_RECV_DATA_ERR		0x08	//接收数据读卡器错误

#define	MSG_CARD_PW_ERR			0x0A	//卡密码错误
#define	MSG_CARD_DAMGED			0x0B	//卡已经损坏
#define	MSG_CARD_WR_ERR			0x0C	//卡写错误
#define MSG_UNKNOW_CARDTYPE		0x0E	//不能识别的卡类型(可能已经物理损坏)

#define	MSG_CARD_DATA_ERR		0x11	//卡数据错误
#define	MSG_CARD_DATA_CHECKSUM	0x12	//卡数据校验合错误
#define	MSG_NO_CARD				0x13	//无卡
#define	MSG_NO_TIME				0x14	//未设置时间
#define	MSG_CARD_INVALID		0x15	//卡无效
#define	MSG_CARD_ERR_EXCEED		0x16	//卡密码已经错误两次
#define	MSG_CARD_FIX_ERROR		0x17	//卡修复出错
#define MSG_MEMBERID_NOT_EQU	0x18	//会员号不匹配
#define MSG_INVALID_COSTTYPE	0x19	//无效的计费模式
#define	MSG_COPYRIGHT_ERR		0x20	//无版权信息或版权信息错误

#define	MSG_IC_BUSY				0x21	//读卡器忙，过3秒再试
#define	MSG_COMMU_CHECKSUM		0x22	//数据在传输过程中出错
#define	MSG_READER_NORESPOND	0x23	//读卡器无响应
#define	MSG_READER_NOT_INIT		0x24	//读卡器未初始化成功
#define	MSG_READER_NOT_SUPPORT	0x25	//读卡器不支持此项操作

#define	MSG_PORT_CANNOTOPEN		0x31	//串口不能打开
#define	MSG_PORT_ALREADYOPEN	0x32	//串口已经打开

#define	MSG_WRITER_NOT_SUPPORT	0x33	//开卡器版本不支持此功能

#define	MSG_DATE_FORMAT_ERROR	0x40	//日期格式错误

#define	MSG_PARAMETER_ERR		0x21	//参数错误
#define	MSG_DATA_LEN_ERR		0x22	//数据长度错误

bool rGetCopyright();

BOOL WINAPI wReadIdCard(LPSTR pszIdCardNumber,BOOL bForceRead);			//读ID卡

BOOL WINAPI wDes(LPSTR pszSource,
									   LPSTR pszDest,
									   LPSTR pszKey,
									   BOOL	bEncrypt);

ULONG WINAPI wIssue(ULONG	ulMemberID,					//会员编号
										  UCHAR	ucMemberTypeID,				//会员类别
										  USHORT usNetbarID,				//开卡网吧编号
										  ULONG	ulExpire,					//从当前时间多少秒此卡有效 0 = 不限时
										  LPSTR	pszOpPass,					//操作密码
										  LPSTR	pszCardID,					//卡ID
										  LPSTR	pszName,					//用户名
										  UCHAR	ucSex,						//用户性别
										  UCHAR	ucIDType,					//证件类别
										  LPSTR	pszIDNumber);				//证件号码
/*
wRead 返回: MSG_CARD_DATA_CHECKSUM 时，数据仍然返回，但有可能是错误的。
需要根据情况重写或者调用相应的接口修复数据。
*/
ULONG WINAPI wRead(
										 LPSTR pszMemberID,					//会员编号
										 LPSTR pszMemberTypeID,				//会员类别
										 LPSTR pszNetbarID,					//开卡网吧编号
										 LPSTR pszPSK,						//PSK
										 LPSTR pszCardID,					//卡ID
										 LPSTR pszFreezeMoney,				//冻结金额
										 LPSTR pszUsedMoney,				//使用了多少金额
										 LPSTR pszRemain,					//冻结剩余金额
										 LPSTR pszRefNo,					//系统参考号
										 LPSTR pszSerialNo,					//本地流水号
										 LPSTR pszCheckinDate,				//登入时间
										 LPSTR pszEndUsingDate,				//最后使用时间
										 //包时
										 LPSTR pszBundleTimeId,				//包时ID
										 LPSTR pszBundleNetbarId,			//包时网吧ID
										 LPSTR pszBundlePcClass,			//包时机器区域
										 LPSTR pszBundlePayType,			//包时类型
										 LPSTR pszBundleAmountTotal,		//包时总金额
										 LPSTR pszBundleAmountUsed,	 		//已消费包时金额
										 LPSTR pszBundleStartTime,			//包时开始时间
										 LPSTR pszBundleEndTime,			//包时结束时间
										 LPSTR pszCardSerialNumber=NULL);	//卡印刷流水码,长度为 15 个字节(包括结束符)，

ULONG WINAPI wEmptyTemp(ULONG	ulMemberID,				//会员编号
										  LPSTR	pszOpPass,					//操作密码
										  LPSTR	pszUID);					//卡片UID

ULONG WINAPI wEmpty(ULONG	ulMemberID,					//会员编号
										  LPSTR	pszOpPass,					//操作密码
										  LPSTR	pszUID);					//卡片UID

ULONG WINAPI wFix(ULONG ulMemberID,					//会员编号  (未使用)
										LPSTR pszOpPass,					//操作密码  (未使用)
										LPSTR pszUID,						//卡片UID   (未使用)
  									    LPSTR pszCardSerialNumber=NULL);	//重写卡片印刷流水码到读卡器 (读卡器 4.6 版本以上)

ULONG WINAPI wReCalChecksum(ULONG	ulMemberID,			//会员编号
										LPSTR pszOpPass,					//操作密码
										LPSTR pszUID,						//卡片UID
										BOOL  bForce,						//强制修复
										ULONG ulUsed,						//已使用金额
										LPSTR pszLastTime);					//最后时间 格式 (2004-01-01 00:00:00)

ULONG WINAPI wCalMac(LPSTR pszBuffer,					//输入 需要计算的数据
										   LPSTR pszMac,					//输出 MAC校验码	8 * 2 个字节
										   LPSTR pszKey,					//输出 加密密钥		8 * 2 个字节
										   ULONG ulLenght);					///输入 需要计算的数据的长度(值为 NULL 就自动计算)

void WINAPI wCalMd5(LPSTR pszIn,						//输入 需要计算的字符串
										  LPSTR pszMd5Hash);				//输出 HASH	值，32 个字节。NULL 结束。

int WINAPI wSetDate(LPSTR pszNewDate);				//日期格式 20041018200101

VOID rGetMsgByrCode(ULONG ulRetCode,					//需要解释的返回值
										  LPSTR pszRetMessage);				//返回值的解释字串

ULONG WINAPI  wInit(ULONG ulPort);					//打开串口并初始化
VOID WINAPI	wClose(VOID);							//关闭串口
BOOL WINAPI	VerifyIdCardNo(char * pszIdCardNo);		//18位身份证校验计算


BOOL WINAPI	wRewriteBundleInfo(ULONG	ulMemberId,
													   USHORT	usTimeId,		//包时ID
													   ULONG	ulSerialNo,		//包时终端流水号
													   ULONG	ulRefNo,		//包时系统参考号
													   USHORT	usNetbarId,		//包时网吧ID
													   UCHAR	ucPcClass,		//包时机器区域
													   UCHAR	ucPayType,		//包时类型
													   ULONG	ulAmountTotal,	//包时总金额
													   ULONG	ulAmountUsed,	//已消费包时金额
													   ULONG	ulFrozenMoney,	//在线已冻结金额
													   ULONG	ulStartTime,	//包时开始时间
													   ULONG	ulEndTime);		//包时结束时间

ULONG WINAPI	ConverTime(LPSTR pszDateString);
USHORT WINAPI	wGetVersion();
BOOL WINAPI	wSetRecvWnd(HWND hRecvWindow);
BOOL WINAPI	wRewritePskCardId(const char * pszCardPsk,	//卡静态密钥
													  unsigned long ulCardId);	//卡ID
//BOOL WINAPI	wChangeInsertMsg(BOOL bSendMsg);			//卡静态密钥

ULONG WINAPI wWrite42(const UCHAR *pucBuffer, ULONG ulPos, ULONG ulLen);
ULONG WINAPI wRead42(UCHAR *pucBuffer, ULONG ulPos, ULONG ulLen);
ULONG WINAPI wChangeInsertMsg(BOOL bSendInsertMessage);

ULONG WINAPI wReadCustom(char *pszBuffer);				//读自定义数据 定长 16 个字节。注意缓冲区最少 16 个字节
ULONG WINAPI wWriteCustom(const char *pszBuffer);			//写自定义数据 定长 16 个字节。注意缓冲区最少 16 个字节

/*调用方法
1.修复 印刷流水码/卡ID/会员号/会员类别/开卡网吧编号
	pszPrintNo/ulCardID/ulMemberId/ucMemberTypeID/usIssueNetbarID 其中一个不空时，重写不为空的值。
	pszPrintNo 以字符串长度判断,长度大于0时，为有效值。

2.修复 [临时扣款信息, 需要提供所有 [参数段二] 的数据字段
3.修复 [包时信息],需要提供 [参数段二] 及 [参数段三] 的数据字段, 并且 [ucCostType] 必需为 1 or 2, 不修复包时 [ucCostType] = 0

*除 pszPrintNo/pszPSK是字符串值之外，其余都是二进制格式
*pszPSK 现阶段未使用.

//写选项
#define OPT_WRITE_CARDINFO		0x00000001			//写卡信息[印刷号|卡号],会员信息[会员号|开卡网吧|会员级别], 参数段一
#define OPT_WRITE_TEMP			0x00000002			//写卡临时信息,参数段二
#define OPT_WRITE_BUNDLE		0x00000004			//写卡包时信息,参数段三
#define OPT_WRITE_CLEAR_TEMP	0x00000008			//清空临时信息
#define OPT_WRITE_CLEAR_BUNDLE	0x00000010			//清空包时信息

例: ulWriteOption = OPT_WRITE_CARDINFO | OPT_WRITE_TEMP | OPT_WRITE_BUNDLE

*/
ULONG WINAPI wWriteCardData(ULONG		ulWriteOption,	//写选项
												  //参数段一,卡信息 用 OPT_WRITE_CARDINFO 选择
												  LPSTR		pszPrintNo,		//印刷流水码
												  ULONG		ulCardID,		//卡ID
												  ULONG		ulMemberId,		//会员号
												  ULONG		ucMemberTypeID,	//会员类别
												  ULONG		usIssueNetbarID,//开卡网吧编号
												  LPSTR		pszPSK,			//PSK
												  //参数段二,临时扣款信息	//用 OPT_WRITE_TEMP 选择
												  ULONG		ucCostType,		//扣款方式
												  ULONG		usNetbarID,		//上机网吧编号
												  ULONG		ucPcClass,		//机器级别
												  ULONG		ulFreezeMoney,	//冻结金额
												  ULONG		ulRemainMoney,	//剩余金额
												  ULONG		ulUsedMoney,	//使用了多少金额
												  ULONG		ulRefNo,		//系统参考号
												  ULONG		ulSerialNo,		//本地流水号
												  ULONG		ulStartTime,	//登入时间
												  ULONG		ulEndTime,		//最后使用时间
												  //参数段三,包时信息		//用 OPT_WRITE_BUNDLE 选择
												  ULONG		usTimeId,			//包时ID
												  ULONG		usBundleNetbarId,	//包时网吧ID
												  ULONG		ucBundlePcClass,	//包时机器区域
												  ULONG		ucPayType,			//包时类型
												  ULONG		ulAmountTotal,		//包时总金额
												  ULONG		ulAmountUsed,		//已消费包时金额
												  ULONG		ulBundleStartTime,	//包时开始时间
												  ULONG		ulBundleEndTime);	//包时结束时间