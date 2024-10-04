if(PL_CMAKE_PLATFORM_WINDOWS_DESKTOP)
    set (PL_3RDPARTY_TRACY_SUPPORT OFF CACHE BOOL "Whether to add support for profiling the engine with Tracy.")
    set (PL_3RDPARTY_TRACY_TRACK_ALLOCATIONS OFF CACHE BOOL "Whether Tracy should track memory allocations.")
else()
    # Tracy currently doesn't compile on Linux
    set (PL_3RDPARTY_TRACY_SUPPORT OFF CACHE BOOL "Whether to add support for profiling the engine with Tracy.")
endif()

mark_as_advanced(FORCE PL_3RDPARTY_TRACY_SUPPORT)

if (NOT PL_3RDPARTY_TRACY_SUPPORT)
    unset(PL_3RDPARTY_TRACY_TRACK_ALLOCATIONS CACHE)
endif()