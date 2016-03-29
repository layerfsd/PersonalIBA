#pragma once

#define		ERROR_IMAGE_TOO_LARGE		-2
#define		ERROR_NETWORK_FAILED		-3

typedef LONG (WINAPI *pfnInit)(LPCSTR serverIP, int serverPort);

typedef void (WINAPI *pfnUnInit)();

typedef LONG (WINAPI* pfnUserLogon)(
									 int idType, 
									 LPCSTR IDNumber, 
									 LPCSTR userName,
									 LPCSTR computerName,
									 int *retCode
									 );

typedef LONG (WINAPI* pfnUserLogout)(int idType, 
									  LPCSTR IDNumber, 
									  LPCSTR userName,
									  LPCSTR computerName
									  );

typedef LONG (WINAPI* pfnRegUser)(
								   int netbarID,
								   int IDType,
								   LPCSTR IDNumber,
								   LPCSTR userName,
								   int sex,
								   LPCSTR countryCode,
								   LPCSTR address,
								   LPCSTR telphone,
								   int memberID,
								   LPCSTR birthDay,
								   LPCSTR openDay,
								   LPCSTR validDate,
								   int    national,/*民族代码*/
								   LPCSTR CertificateOffice,/*发证单位*/
								   int  CardType,/*卡类型*/
								   LPCSTR serialNum,/*印刷号*/
								   int *retCode);

typedef LONG (WINAPI* pfnSendRegImage)(
										LPCSTR IDNumber,
										int IDType,
										LPCSTR userName,
										char *image,
										int imageSize,
										int *retCode);


typedef LONG (WINAPI* pfnSendIDImage)(
									   LPCSTR IDNumber,
									   int IDType,
									   LPCSTR userName,
									   char *image,
									   int imageSize,
									   int *retCode);

typedef LONG (WINAPI* pfnIsRegisted)(
									  LPCSTR userName,
									  char* netbarID,
									  LPCSTR IDNumber, 
									  int IDType,
									  int *retCode);

typedef LONG (WINAPI* pfnServerUserLogon)(
	int idType,
	LPCSTR IDNumber,
	LPCSTR userName,
	LPCSTR computerName,
	char *imageBuffer,
	int imgSize,
	int *retCode);

typedef LONG (WINAPI* pfnPing)( void );

typedef LONG (WINAPI* pfnSendRegImageEx)(
										LPCTSTR MemberId,
										LPCSTR IDNumber,
										int IDType,
										LPCSTR userName,
										char *image,
										int imageSize,
										int *retCode);

typedef LONG (WINAPI* pfnSendIDImageEx)(
										LPCTSTR MemberId,
										LPCSTR IDNumber,
										int IDType,
										LPCSTR userName,
										char *image,
										int imageSize,
										int *retCode);

typedef LONG (WINAPI* pfnGetImageRemote)(
										char* szMemberId,
										char* szName,
										char* szIDNumber,
										int nIDType,
										int nImgType,
										byte* szImgBuf,
										DWORD& dwBufLen);

typedef LONG (WINAPI* pfnSendIDImageRZX2008)(
										LPCTSTR MemberId,
										LPCSTR IDNumber,
										int IDType,
										LPCSTR userName,
										LPCSTR CertificateOffice,
										LPCSTR RzxSn,
										LPCSTR CMSn,
										char *image,
										int imageSize,
										int *retCode);

typedef LONG (WINAPI* pfnSendRegImageRZX2008)(
										LPCTSTR MemberId,
										LPCSTR IDNumber,
										int IDType,
										LPCSTR userName,
										LPCSTR CertificateOffice,
										LPCSTR RzxSn,
										LPCSTR CMSn,
										char *image,
										int imageSize,
										int *retCode);

typedef LONG (WINAPI* pfnUserLogonRZX2008)(
										int idType, 
										LPCSTR IDNumber, 
										LPCSTR CertificateOffice,
										LPCSTR userName,
										LPCSTR computerName,
										DWORD  dwIPAddr,
										LPCSTR RzxSn,
										LPCSTR CMSn,
										int *retCode);

typedef LONG (WINAPI* pfnUserLogoutRZX2008)(
										int idType, 
										LPCSTR IDNumber, 
										LPCSTR CertificateOffice,
										LPCSTR userName,
										LPCSTR computerName,
										DWORD  dwIPAddr,
										LPCSTR RzxSn,
										LPCSTR CMSn
										);


typedef LONG (WINAPI* pfnDeRegUser)(
								  int netbarID,
								  int IDType,
								  LPCSTR IDNumber,
								  LPCSTR userName,
								  int sex,
								  LPCSTR countryCode,
								  LPCSTR address,
								  LPCSTR telphone,
								  int memberID,
								  LPCSTR birthDay,
								  LPCSTR openDay,
								  LPCSTR validDate,
								  int    national,/*民族代码*/
								  LPCSTR CertificateOffice,/*发证单位*/
								  int  CardType,/*卡类型*/
								  LPCSTR serialNum,/*印刷号*/
								  int *retCode);
class CAuditInterface{

public:

	CAuditInterface(){}
	
	~CAuditInterface(){}

	BOOL Load();

public:

	typedef struct tag_RegUserInfo
	{
		int netbarID;
		int IDType;
		CStringA strIDNumber;
		CStringA strUserName;
		int sex;
		CStringA strCountryCode;
		CStringA strAddress;
		CStringA strTelphone;
		int memberID;
		CStringA strBirthDay;
		CStringA strOpenDay;
		CStringA strValidDate;
		int    national;/*民族代码*/
		CStringA strCertificateOffice;/*发证单位*/
		int  CardType;/*卡类型*/
		CStringA strSerialNum;/*印刷号*/
		int retCode;
	}RegUserInfo;

	//定制化函数
	BOOL CustomRegUser(RegUserInfo& regUserInfo);


	//***************从动态库的读取的函数******************//
	pfnInit init_;
	pfnUnInit uninit_;
	pfnUserLogon userLogon_;
	pfnUserLogout logout_;
	pfnRegUser regUser_;
	pfnSendRegImage sendRegImage_;
	pfnSendIDImage sendIDImage_;
	pfnIsRegisted IsRegisted_;
	pfnServerUserLogon ServerUserLogon_;
	pfnPing Ping_;
	pfnSendRegImageEx sendRegImageEx_;
	pfnSendIDImageEx sendIDImageEx_;
	pfnGetImageRemote GetImageRemote_;
	pfnSendRegImageRZX2008 SendRegImageRZX2008_;
	pfnSendIDImageRZX2008  SendIDImageRZX2008_;
	pfnUserLogonRZX2008 UserLogonRZX2008_;
	pfnUserLogoutRZX2008 UserLogoutRZX2008_;
	pfnDeRegUser pfnDeRegUser_;
	HMODULE hdll;
};