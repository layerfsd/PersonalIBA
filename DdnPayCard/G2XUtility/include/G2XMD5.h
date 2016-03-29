#pragma once

namespace G2XUtility{

class G2X_CLASSES CG2XMD5
{
public:
	static void CalculateMD5(const char* lpStr, int iLen, char* lpOut /* buffer size > 32*/);
	static void CalculateMD5(const void* lvData, unsigned int iSize, char* lpOut /* buffer size > 32*/);
	static void CalculateFileMD5(const char* lpszFileName, char* lpOut /* buffer size > 32*/);
};

}