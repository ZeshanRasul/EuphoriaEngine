# find the folder into which the OpenXR loader has been installed

# early out, if this target has been created before
if(TARGET plOpenXR::Loader)
	return()
endif()

set(PL_OPENXR_LOADER_DIR "PL_OPENXR_LOADER_DIR-NOTFOUND" CACHE PATH "Directory of OpenXR loader installation")
set(PL_OPENXR_HEADERS_DIR "PL_OPENXR_HEADERS_DIR-NOTFOUND" CACHE PATH "Directory of OpenXR headers installation")
set(PL_OPENXR_PREVIEW_DIR "" CACHE PATH "Directory of OpenXR preview include root")
set(PL_OPENXR_REMOTING_DIR "" CACHE PATH "Directory of OpenXR remoting installation")
mark_as_advanced(FORCE PL_OPENXR_LOADER_DIR)
mark_as_advanced(FORCE PL_OPENXR_HEADERS_DIR)
mark_as_advanced(FORCE PL_OPENXR_PREVIEW_DIR)
mark_as_advanced(FORCE PL_OPENXR_REMOTING_DIR)

pl_pull_compiler_and_architecture_vars()

if((PL_OPENXR_LOADER_DIR STREQUAL "PL_OPENXR_LOADER_DIR-NOTFOUND") OR(PL_OPENXR_LOADER_DIR STREQUAL "") OR(PL_OPENXR_HEADERS_DIR STREQUAL "PL_OPENXR_HEADERS_DIR-NOTFOUND") OR(PL_OPENXR_HEADERS_DIR STREQUAL "") OR(PL_OPENXR_REMOTING_DIR STREQUAL "PL_OPENXR_REMOTING_DIR-NOTFOUND") OR(PL_OPENXR_REMOTING_DIR STREQUAL ""))
	pl_nuget_init()
	execute_process(COMMAND ${NUGET} restore ${PL_ROOT}/Code/EnginePlugins/OpenXRPlugin/packages.config -PackagesDirectory ${CMAKE_BINARY_DIR}/packages
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
	set(PL_OPENXR_LOADER_DIR "${CMAKE_BINARY_DIR}/packages/OpenXR.Loader.1.0.10.2" CACHE PATH "Directory of OpenXR loader installation" FORCE)
	set(PL_OPENXR_HEADERS_DIR "${CMAKE_BINARY_DIR}/packages/OpenXR.Headers.1.0.10.2" CACHE PATH "Directory of OpenXR headers installation" FORCE)
	set(PL_OPENXR_REMOTING_DIR "${CMAKE_BINARY_DIR}/packages/Microsoft.Holographic.Remoting.OpenXr.2.4.0" CACHE PATH "Directory of OpenXR remoting installation" FORCE)
endif()

if(PL_CMAKE_PLATFORM_WINDOWS_UWP)
	set(OPENXR_DYNAMIC ON)
	find_path(PL_OPENXR_HEADERS_DIR include/openxr/openxr.h)

	if(PL_CMAKE_ARCHITECTURE_ARM)
		if(PL_CMAKE_ARCHITECTURE_64BIT)
			set(OPENXR_BIN_PREFIX "arm64_uwp")
		else()
			set(OPENXR_BIN_PREFIX "arm_uwp")
		endif()
	else()
		if(PL_CMAKE_ARCHITECTURE_64BIT)
			set(OPENXR_BIN_PREFIX "x64_uwp")
		else()
			set(OPENXR_BIN_PREFIX "Win32_uwp")
		endif()
	endif()

elseif(PL_CMAKE_PLATFORM_WINDOWS_DESKTOP)
	set(OPENXR_DYNAMIC ON)
	find_path(PL_OPENXR_HEADERS_DIR include/openxr/openxr.h)

	if(PL_CMAKE_ARCHITECTURE_64BIT)
		set(OPENXR_BIN_PREFIX "x64")
		find_path(PL_OPENXR_REMOTING_DIR build/native/include/openxr/openxr_msft_holographic_remoting.h)
	else()
		set(OPENXR_BIN_PREFIX "Win32")
	endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(plOpenXR DEFAULT_MSG PL_OPENXR_LOADER_DIR)
find_package_handle_standard_args(plOpenXR DEFAULT_MSG PL_OPENXR_HEADERS_DIR)
find_package_handle_standard_args(plOpenXR DEFAULT_MSG PL_OPENXR_REMOTING_DIR)

if(PLOPENXR_FOUND)
	add_library(plOpenXR::Loader SHARED IMPORTED)

	if(OPENXR_DYNAMIC)
		set_target_properties(plOpenXR::Loader PROPERTIES IMPORTED_LOCATION "${PL_OPENXR_LOADER_DIR}/native/${OPENXR_BIN_PREFIX}/release/bin/openxr_loader.dll")
		set_target_properties(plOpenXR::Loader PROPERTIES IMPORTED_LOCATION_DEBUG "${PL_OPENXR_LOADER_DIR}/native/${OPENXR_BIN_PREFIX}/release/bin/openxr_loader.dll")
	endif()

	set_target_properties(plOpenXR::Loader PROPERTIES IMPORTED_IMPLIB "${PL_OPENXR_LOADER_DIR}/native/${OPENXR_BIN_PREFIX}/release/lib/openxr_loader.lib")
	set_target_properties(plOpenXR::Loader PROPERTIES IMPORTED_IMPLIB_DEBUG "${PL_OPENXR_LOADER_DIR}/native/${OPENXR_BIN_PREFIX}/release/lib/openxr_loader.lib")

	set_target_properties(plOpenXR::Loader PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${PL_OPENXR_HEADERS_DIR}/include")

	if(NOT PL_OPENXR_PREVIEW_DIR STREQUAL "")
		set_target_properties(plOpenXR::Loader PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${PL_OPENXR_HEADERS_DIR}/include")
	endif()

	pl_uwp_mark_import_as_content(plOpenXR::Loader)

	if(PL_CMAKE_PLATFORM_WINDOWS_DESKTOP AND PL_CMAKE_ARCHITECTURE_64BIT)

		add_library(plOpenXR::Remoting INTERFACE IMPORTED)

		if(PL_CMAKE_PLATFORM_WINDOWS_UWP)
			list(APPEND REMOTING_ASSETS "${PL_OPENXR_REMOTING_DIR}/build/native/bin/x64/uwp/RemotingXR.json")
			list(APPEND REMOTING_ASSETS "${PL_OPENXR_REMOTING_DIR}/build/native/bin/x64/uwp/Microsoft.Holographic.AppRemoting.OpenXr.dll")
		else()
			list(APPEND REMOTING_ASSETS "${PL_OPENXR_REMOTING_DIR}/build/native/bin/x64/Desktop/RemotingXR.json")
			list(APPEND REMOTING_ASSETS "${PL_OPENXR_REMOTING_DIR}/build/native/bin/x64/Desktop/Microsoft.Holographic.AppRemoting.OpenXr.dll")
		endif()

		set_target_properties(plOpenXR::Remoting PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${PL_OPENXR_REMOTING_DIR}/build/native/include")
		set_target_properties(plOpenXR::Remoting PROPERTIES INTERFACE_SOURCES "${REMOTING_ASSETS}")

		set_property(SOURCE ${REMOTING_ASSETS} PROPERTY VS_DEPLOYMENT_CONTENT 1)
		set_property(SOURCE ${REMOTING_ASSETS} PROPERTY VS_DEPLOYMENT_LOCATION "")

	endif()

	
endif()

unset(OPENXR_DYNAMIC)
unset(OPENXR_BIN_PREFIX)