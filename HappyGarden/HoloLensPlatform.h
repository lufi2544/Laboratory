#pragma once


#include "GenericPlatform.h"

#define PLATFORM_HOLOLENS




/*
 * Windows specific types
 **/
struct FHoloLensTypes : public FGenericPlatformTypes
{

	// defined in windefs.h, even though this is equivalent, the compiler doesn't think so
	typedef unsigned long	DWORD;

#ifdef _WIN64
	typedef unsigned __int64   SIZE_T;
	typedef __int64		     SSIEZE_T;
#else
	typedef unsigned long	   SIZE_T;
	typedef long			  SSIZE_T;
#endif // _WIN64

	typedef decltype(__nullptr) TYPE_OF_NULLPTR;

};



