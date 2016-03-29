#pragma once

#ifdef DDN_CAPTURE_VIDEO
#define DDN_DLLAPI _declspec(dllexport)
#else
#define DDN_DLLAPI _declspec(dllimport)
#endif

class CCaptureVideo;

// lpDeviceName: 设备名称
// pParam: 设置回调函数时传入的参数
typedef void (*DDNEnumDevicesCallback)(const char* lpDeviceName, void* pParam);

//=========================CCapVideoInf的声明===============================
class DDN_DLLAPI CCapVideoInf
{
public:
	CCapVideoInf();
	~CCapVideoInf();
	
	int EnumDevices(DDNEnumDevicesCallback lpCallback, void* pParam); 

	// 程序初始化
	// iDevice传递要使用的驱动器号
	// 可直接由EnumDevices枚举的顺序的序号作为iDeviceID, (>=0)
	HRESULT Init(int iDeviceID,HWND hVideoWnd); 
	
	// 停止预览
	BOOL Stop();

	// 截取一张jpg图片，传入的参数是要保存的照片的文件名
	// 不需要传递文件后缀
	BOOL CapOneImage(const char* szFileName);

	// 设置截取后的图片的保存路径
	void SetBitmapSavePath( const char* szBitPath );

	void ResizeVideoPos();
		
	// 返回是否打开
	BOOL IsOpened();

	// 传入一个全路径，检测是否存在
	BOOL FileIsExist( const char* szFileName);

private:
	CCaptureVideo *m_pCapVideo;
};
