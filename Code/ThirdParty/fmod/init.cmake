######################################
### Fmod support
######################################

if(PL_CMAKE_PLATFORM_WINDOWS OR PL_CMAKE_PLATFORM_LINUX)
  set (PL_BUILD_FMOD ON CACHE BOOL "Whether support for FMOD should be added")
else()
  set (PL_BUILD_FMOD OFF CACHE BOOL "Whether support for FMOD should be added" FORCE)
endif()


set (PL_FMOD_USE_CUSTOM_INSTALLATION OFF CACHE BOOL "If enabled, the built-in FMOD version is ignored and any installed version is used instead. Clear PL_FMOD_SDK_LOCATION to let CMake reevaluate the path.")

mark_as_advanced(FORCE PL_FMOD_USE_CUSTOM_INSTALLATION)

######################################
### pl_requires_fmod()
######################################

macro(pl_requires_fmod)

	pl_requires_one_of(PL_CMAKE_PLATFORM_WINDOWS PL_CMAKE_PLATFORM_LINUX)
	pl_requires(PL_BUILD_FMOD)

endmacro()

######################################
### pl_link_target_fmod(<target>)
######################################

function(pl_link_target_fmod TARGET_NAME)

	pl_requires_fmod()

    target_link_libraries(${TARGET_NAME} PRIVATE plFmod::Studio)

    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:plFmod::Studio> $<TARGET_FILE_DIR:${TARGET_NAME}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:plFmod::Core> $<TARGET_FILE_DIR:${TARGET_NAME}>
  )

endfunction()

