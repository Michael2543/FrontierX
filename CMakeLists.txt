cmake_minimum_required(VERSION 3.25)

project(FrontierX LANGUAGES CXX)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
	message(FATAL_ERROR "The initial FrontierX CMake target supports Linux only")
endif()

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
	message(FATAL_ERROR "The initial FrontierX CMake target requires a 64-bit toolchain")
endif()

if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|AMD64)$")
	message(FATAL_ERROR "The initial FrontierX CMake target supports x86_64 only")
endif()

function(frontierx_enable_strict_warnings target_name)
	set_target_properties(${target_name} PROPERTIES CXX_EXTENSIONS OFF)
	if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
		target_compile_options(${target_name}
			PRIVATE
				-Wall
				-Wextra
				-Wpedantic
				-Werror
		)
	endif()
endfunction()

add_executable(frontierx_layout_probe
	Tools/portability/layout_probe.cpp
)

target_include_directories(frontierx_layout_probe
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/Code/App/DInclude
		${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Include
)

target_compile_features(frontierx_layout_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_layout_probe)

add_library(frontierx_tinyxml STATIC
	Code/Libs/Src/XML/tinystr.cpp
	Code/Libs/Src/XML/tinystr.h
	Code/Libs/Src/XML/tinyxml.cpp
	Code/Libs/Src/XML/tinyxml.h
	Code/Libs/Src/XML/tinyxmlerror.cpp
	Code/Libs/Src/XML/tinyxmlparser.cpp
)

target_include_directories(frontierx_tinyxml
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Src/XML
)

target_compile_features(frontierx_tinyxml PUBLIC cxx_std_17)
frontierx_enable_strict_warnings(frontierx_tinyxml)

add_executable(frontierx_tinyxml_probe
	Tools/portability/tinyxml_probe.cpp
)

target_link_libraries(frontierx_tinyxml_probe PRIVATE frontierx_tinyxml)
target_compile_features(frontierx_tinyxml_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_tinyxml_probe)

add_library(frontierx_platform STATIC
	Code/Libs/Include/FrontierX/DataRoot.h
	Code/Libs/Include/FrontierX/PlatformTime.h
	Code/Libs/Src/Platform/DataRoot.cpp
	Code/Libs/Src/Platform/ProfileDocument.cpp
	Code/Libs/Src/Platform/UtfMetadata.cpp
	Code/Libs/Src/Platform/PlatformTime.cpp
)

target_include_directories(frontierx_platform
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Include
)

target_compile_features(frontierx_platform PUBLIC cxx_std_17)
frontierx_enable_strict_warnings(frontierx_platform)

add_executable(frontierx_platform_time_probe
	Tools/portability/platform_time_probe.cpp
)

target_link_libraries(frontierx_platform_time_probe PRIVATE frontierx_platform)
target_compile_features(frontierx_platform_time_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_platform_time_probe)

add_executable(frontierx_data_root_probe
	Tools/portability/data_root_probe.cpp
)

target_link_libraries(frontierx_data_root_probe PRIVATE frontierx_platform)
target_compile_features(frontierx_data_root_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_data_root_probe)

# The production DACOM adapter and this probe share the same inline parser core.
add_library(frontierx_profile_buffer INTERFACE)
target_include_directories(frontierx_profile_buffer INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Include)
target_compile_features(frontierx_profile_buffer INTERFACE cxx_std_17)
add_executable(frontierx_profile_buffer_probe Tools/portability/profile_buffer_probe.cpp)
target_link_libraries(frontierx_profile_buffer_probe PRIVATE frontierx_profile_buffer)
frontierx_enable_strict_warnings(frontierx_profile_buffer_probe)

add_executable(frontierx_profile_document_probe Tools/portability/profile_document_probe.cpp)
target_link_libraries(frontierx_profile_document_probe PRIVATE frontierx_platform)
frontierx_enable_strict_warnings(frontierx_profile_document_probe)

add_executable(frontierx_headless Code/Launcher/Headless.cpp)
target_link_libraries(frontierx_headless PRIVATE frontierx_platform)
frontierx_enable_strict_warnings(frontierx_headless)

include(CTest)
if(BUILD_TESTING)
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    add_test(NAME portability.headless COMMAND ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability/test_headless.py $<TARGET_FILE:frontierx_headless>)
	add_test(NAME portability.profile_document COMMAND frontierx_profile_document_probe)
	add_test(NAME portability.profile_buffer COMMAND frontierx_profile_buffer_probe)
	add_test(NAME portability.layout COMMAND frontierx_layout_probe)
	add_test(NAME portability.tinyxml COMMAND frontierx_tinyxml_probe)
	add_test(NAME portability.platform_time COMMAND frontierx_platform_time_probe)
	add_test(NAME portability.data_root COMMAND frontierx_data_root_probe --self-test)
endif()