#pragma once
#include "servletmethod.h"

// 2011/10/24-8230-gxx: 添加，包房查询接口
// class: CConsumeListMethodEx 
//
namespace NS_SERVLET
{

class CNetbarRoomInfo
{
public :
	
	CNetbarRoomInfo();

	// 包房ID （即机器区域ID）
	UINT GetPcClass();  
	
	// 包房名称
	CString GetAreaName();

	// 包房，主账号ID
	UINT GetMemberId();

	// 包房的开通时间
	CString GetBeginTime();

public :

	CRespFieldList  m_RespFields;	//!< 返回值列表

	friend class CNetbarRoomMethod;

};

typedef CArray<CNetbarRoomInfo, CNetbarRoomInfo&> CNetbarRoomInfoArray;

class CNetbarRoomMethod : public CServletMethod
{
public :
	CNetbarRoomMethod(void);
	~CNetbarRoomMethod(void);

protected :

	void AddParams();
	void AddRespFields();

	BOOL DecodeResp(CString strResp);

	CNetbarRoomInfoArray m_RoomInfoList;

public :
	/**
	*操作员
	*/
	void SetOperator(CString newVal);

public :

	/**
	*记录数量, 目前最大100条
	*/
	UINT GetRecordCount();

	// 包房ID, 即区域ID
	UINT GetRoomID(INT nIndex);
	
	// 包房名称,即区域名称
	CString GetRoomName(INT nIndex);

	// 用户ID
	UINT GetMemberID(INT nIndex);

	// 开通时间
	CString GetOpenTime(INT nIndex);
};

}