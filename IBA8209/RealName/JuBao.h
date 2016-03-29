#pragma once

class CJubaoDelegate
{
public:
	virtual void DoCheckEnd(char* username,char* idcardno ,DWORD checkresult,char * transNumber,char * bitmap){}
};

class CJuBao
{
public:
	CJuBao();
	~CJuBao();
	int RegCallBackFunc();
	int InitInterface(std::string netbarid, std::string realid, std::string jubaosn); 
	static void CheckEnd( char* username,char* idcardno ,DWORD checkresult,char * transNumber,char * bitmap);
	int VerifyUserInfo(std::string name,std::string idCard, std::string stPhotoBuf);
	void SetJubaoDelegate(CJubaoDelegate* pDelegate) { m_pJubaoDelegate = pDelegate; }
	static std::string Decode(const char* Data,int DataByte,int& OutByte);
	static std::string Encode(const char* Data,int DataByte);
private:
	HINSTANCE m_hInst;
	BOOL m_bInitSuccess;
	static CJubaoDelegate *m_pJubaoDelegate;
	//static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; 
};
    