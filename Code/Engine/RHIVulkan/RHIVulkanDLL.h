#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Types/UniquePtr.h>
#include <RHI/RHIDLL.h>

#include <vulkan/vulkan.hpp>

// Configure the DLL Import/Export Define
#if PL_ENABLED(PL_COMPILE_ENGINE_AS_DLL)
#  ifdef BUILDSYSTEM_BUILDING_RHIVULKAN_LIB
#    define PL_RHIVULKAN_DLL __declspec(dllexport)
#  else
#    define PL_RHIVULKAN_DLL __declspec(dllimport)
#  endif
#else
#  define PL_RHIVULKAN_DLL
#endif
