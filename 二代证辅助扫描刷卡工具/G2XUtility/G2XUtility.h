#pragma once

#include <vector>
#include <functional>
#include <string>
#include <map>
#include <algorithm>

#ifdef G2X_CLASSES
#undef G2X_CLASSES
#endif 

#ifdef G2X_CLASSES_EXPORT
#define G2X_CLASSES __declspec(dllexport)
#else
#define G2X_CLASSES __declspec(dllimport)
#endif

#include ".\\include\\G2XPropertyDefine.h"
#include ".\\include\\G2XStatusArray.h"
#include ".\\include\\G2XFunction.h"
#include ".\\include\\GxxString.h"
#include ".\\include\\G2XCommonTool.h"
#include ".\\include\\G2XMD5.h"
#include ".\\include\\G2XDestructorRunner.h"
#include ".\\include\\G2XDelegater.h"
#include ".\\include\\ServiceCheck.h"
#include ".\\include\\G2XEnumerator.h"