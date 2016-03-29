
#pragma once

#define DDN_EXT_CLASS __declspec(dllimport)

#include "MDataXSender.h"
#include "MDataXReceiver.h"

#include "EDataXSender.h"
#include "EDataXReceiver.h"

#ifdef _DEBUG

	#ifdef _UNICODE
		#pragma comment(lib, "DdnDataXUd.lib")
	#else
		#pragma comment(lib, "DdnDataXd.lib")
	#endif

#else

	#ifdef _UNICODE
		#pragma comment(lib, "DdnDataXU.lib")
	#else
		#pragma comment(lib, "DdnDataX.lib")
	#endif

#endif

