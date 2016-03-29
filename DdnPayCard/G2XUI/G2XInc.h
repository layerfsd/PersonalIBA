#pragma once

#ifdef G2X_CLASSES
#undef G2X_CLASSES
#endif 

#ifdef G2X_CLASSES_EXPORT
#define G2X_CLASSES __declspec(dllexport)
#else
#define G2X_CLASSES __declspec(dllimport)
#endif

#include "G2X.h"
