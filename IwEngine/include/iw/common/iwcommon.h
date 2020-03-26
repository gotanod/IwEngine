#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IW_BUILD_DLL
#		define IWCOMMON_API __declspec(dllexport)
#else
#		define IWCOMMON_API __declspec(dllimport)
#	endif
#endif

#include "iw/reflection/Reflect.h"

#ifdef REFLECTING
#	undef  IWCOMMON_API
#	define IWCOMMON_API
#endif
