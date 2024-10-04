# #####################################
# ## pl_requires_renderer()
# #####################################

macro(pl_requires_renderer)
	if(PL_BUILD_EXPERIMENTAL_WEBGPU)
		pl_requires_webgpu()
	endif()
	if(PL_BUILD_EXPERIMENTAL_VULKAN)
		pl_requires_vulkan()
	endif()
	if(PL_CMAKE_PLATFORM_WINDOWS)
		pl_requires_d3d()
	endif()
endmacro()

# #####################################
# ## pl_add_renderers(<target>)
# ## Add all required libraries and dependencies to the given target so it has access to all available renderers.
# #####################################
function(pl_add_renderers TARGET_NAME)
	# PLATFORM-TODO
	if(PL_BUILD_EXPERIMENTAL_VULKAN)
		target_link_libraries(${TARGET_NAME}
			PRIVATE
			RendererVulkan
		)

		if (TARGET ShaderCompilerVulkan)
			add_dependencies(${TARGET_NAME}
					ShaderCompilerVulkan
			)
		endif()
	endif()

	if(PL_BUILD_EXPERIMENTAL_WEBGPU)
		target_link_libraries(${TARGET_NAME}
				PRIVATE
				RendererWebGPU
		)
		if (TARGET ShaderCompilerWebGPU)
			add_dependencies(${TARGET_NAME}
					ShaderCompilerWebGPU
			)
		endif()
	endif()


	if(PL_CMAKE_PLATFORM_WINDOWS)
		target_link_libraries(${TARGET_NAME}
			PRIVATE
			RendererDX11
		)
		pl_link_target_dx11(${TARGET_NAME})

		if (TARGET ShaderCompilerHLSL)
			add_dependencies(${TARGET_NAME}
				ShaderCompilerHLSL
			)
		endif()
	endif()
endfunction()