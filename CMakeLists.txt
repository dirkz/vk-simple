# CMakeList.txt : CMake project for vkdeck, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("vkdeck")

# Add source to this project's executable.
if (WIN32)
	add_executable (${PROJECT_NAME} WIN32 "main.cpp" )
else()
	add_executable (${PROJECT_NAME} "main.cpp" )
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET vkdeck PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.

# Set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
# This should help with dynamically linking to SDL3.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

find_package(Vulkan REQUIRED)

add_subdirectory(glm)
add_subdirectory(SDL-Hpp EXCLUDE_FROM_ALL)
add_subdirectory(engine)
add_subdirectory(shaders)
add_subdirectory(textures)

add_dependencies(${PROJECT_NAME} shaders)

target_precompile_headers(${PROJECT_NAME} REUSE_FROM engine)

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC engine Vulkan::Vulkan glm::glm SDL-Hpp)
