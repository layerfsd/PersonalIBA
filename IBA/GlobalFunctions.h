#pragma once
#include "LocalCenter\gxx_base.h"

namespace IBAGlobal{

// 随机创建一个用户姓名
CString MakeUserName();

CString GetCurrentDateTime();


CString StringValueForKey(const char* key, GxxDictionaryPtr dicPtr);





















}