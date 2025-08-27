add_library(engine OBJECT
	Engine.h
	Engine.cpp
	IVulkanWindow.h
	SDLVulkanWindow.h
	SDLVulkanWindow.cpp
	QueueFamilyIndices.h
	QueueFamilyIndices.cpp
	SwapchainSupportDetails.h
	SwapchainSupportDetails.cpp
	Swapchain.h
	Swapchain.cpp
	ShaderModuleLoader.h
	ShaderModuleLoader.cpp
	FrameData.h
	FrameData.cpp
	Vertex.h
	Buffer.h
	Buffer.cpp
	Vma.cpp
	Vma.h
	VmaBuffer.h
	VmaBuffer.cpp
	VmaHelpers.h)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET engine PROPERTY CXX_STANDARD 20)
endif()

target_precompile_headers(engine PUBLIC stdafx.h)

target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../stb)
target_include_directories(engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tinyobjloader)

target_link_libraries(engine PRIVATE SDL-Hpp Vulkan::Vulkan glm::glm)
