#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Types/Id.h>
#include <Foundation/Types/RefCounted.h>
#include <Foundation/Types/SharedPtr.h>

// Configure the DLL Import/Export Define
#if PL_ENABLED(PL_COMPILE_ENGINE_AS_DLL)
#  ifdef BUILDSYSTEM_BUILDING_RHI_LIB
#    define PL_RHI_DLL __declspec(dllexport)
#  else
#    define PL_RHI_DLL __declspec(dllimport)
#  endif
#else
#  define PL_RHI_DLL
#endif
