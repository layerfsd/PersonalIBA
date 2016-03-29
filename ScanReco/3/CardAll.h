
#ifndef _CARDALL3_H_
#define _CARDALL3_H_



typedef struct
{
	char name[40];   //姓名
	char sex[10];     //性别
	char people[20];  //民族，护照识别时此项为空
	char birthday[50]; //出生日期
	char address[200]; //地址，在识别护照时导出的是国籍简码、
	char signdate[50]; //签发日期，在识别护照时导出的是有效期至 
	char validterm[10];  //有效期，在识别护照时为空，
	char number[40];   //证件编码，在识别护照时为护照号 ；识别港澳通行证内页时为港澳证件号码
	char other[100];    //备用，在识别护照时导出护照类型,识别港澳通行证封页时证件号；
}IDCARD_ALL3;

/*
//单字切分信息结构
typedef struct
{
	int nSRow; //纵向起始位置
	int nERow; //纵向终止位置
	int nSCol; //横向起始位置
	int nECol; //横向终止位置
	int nWidth; //图像块宽度
	int nHeight; //图像块高度

}SQ_RECOGCHARINFO, far* LPSQ_RECOGCHARINFO;

//单字识别信息结构
typedef struct
{
	BOOL bAccept; //识别结果是否可以接受
	BYTE FontType; // --没用
	WORD wCode [10]; //识别结果码
	int nErrDist[10]; //识别结果置信度，值越小置信度越高。

}ZQ_RECOGCHARRESULT, far* LPZQ_RECOGCHARRESULT;

//区域信息结构
typedef struct
{
	RECT rc; //该区域在图像中所处位置
	int nCharCount; //该区域中字符数目
	SQ_RECOGCHARINFO RecogInfo[50]; //该区域中所有字符的切分信息
	ZQ_RECOGCHARRESULT RecogResult[50]; //该区域中所有字符的识别信息

}ST_REGION, FAR* LPST_REGION;

//身份证结构
typedef struct
{
	RECT rcPhoto;
	ST_REGION name;
	ST_REGION sex;
	ST_REGION people;
	ST_REGION birthday;
	ST_REGION address[5];
	ST_REGION signdate;
	ST_REGION validterm;
	ST_REGION number;
	int address_line_count;
}ST_IDCARD, FAR* LPST_IDCARD;
*/


typedef int   (WINAPI Fun_V3_LoadIdcardLibrary)();
typedef void  (WINAPI Fun_V3_FreeIdcardLibrary)();
typedef int   (WINAPI Fun_V3_ScanImageEx)( char* pchFilePath, short nButtonType );
typedef int   (WINAPI Fun_V3_RecogIdcardExALL)( LPTSTR imgfile_name, IDCARD_ALL3 & card, bool bSaveResultFile, int nHandPrint, int nContent, char* HeadImgFName, short nButtonType );
typedef int   (WINAPI Fun_V3_RecogIdcardOnlyALL)( LPCTSTR imgfile_name, IDCARD_ALL3 & card, bool bSaveResultFile, int nHandPrint, int nContent, char* HeadImgFName );
typedef int   (WINAPI Fun_V3_RecogDPOnlyALL)( LPCTSTR imgfile_name, IDCARD_ALL3 & card, bool bSaveResultFile, int nContent, char* HeadImgFName );
typedef int   (WINAPI Fun_V3_RecogDPALL)( LPTSTR imgfile_name, IDCARD_ALL3 & card, bool bSaveResultFile, int nContent, char* HeadImgFName );
typedef int   (WINAPI Fun_V3_RecogPassPortOnlyALL)( LPCTSTR imgfile_name, IDCARD_ALL3 & card, bool bSaveResultFile, int nContent, char* HeadImgFName );
typedef int   (WINAPI Fun_V3_RecogPassPortALL)( LPTSTR imgfile_name, IDCARD_ALL3 & card, bool bSaveResultFile, int nContent, char* HeadImgFName );
typedef int   (WINAPI Fun_V3_RecogNewIdcardOnlyALL)( LPCTSTR imgfile_name, IDCARD_ALL3 & card, bool bSaveResultFile, int nContent, char* HeadImgFName );
typedef int   (WINAPI Fun_V3_RecogNewIdcardALL)( LPTSTR imgfile_name, IDCARD_ALL3& card, bool bSaveResultFile, int nContent, char* HeadImgFName );
typedef int   (WINAPI Fun_V3_GetButtonDownType)(void);

#endif