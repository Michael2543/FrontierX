cmake_minimum_required(VERSION 3.25)

project(FrontierX LANGUAGES CXX)

option(FRONTIERX_BUILD_SDL3 "Build the isolated SDL3 window/input backend and probe" OFF)

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

add_library(frontierx_profile_document STATIC
	Code/Libs/Include/FrontierX/ProfileBuffer.h
	Code/Libs/Include/FrontierX/ProfileDocument.h
	Code/Libs/Src/Platform/ProfileDocument.cpp
)

target_include_directories(frontierx_profile_document
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Include)
target_compile_features(frontierx_profile_document PUBLIC cxx_std_17)
frontierx_enable_strict_warnings(frontierx_profile_document)

add_library(frontierx_platform STATIC
	Code/Libs/Include/FrontierX/DataRoot.h
	Code/Libs/Include/FrontierX/EventQueue.h
	Code/Libs/Include/FrontierX/LegacyPreferenceValues.h
	Code/Libs/Include/FrontierX/NativeUserSettings.h
	Code/Libs/Include/FrontierX/NativeUserSettingsOwner.h
	Code/Libs/Include/FrontierX/PlatformTime.h
	Code/Libs/Include/FrontierX/PreferenceStore.h
	Code/Libs/Include/FrontierX/SettingsPath.h
	Code/Libs/Include/FrontierX/StartupDataRoot.h
	Code/Libs/Src/Platform/DataRoot.cpp
	Code/Libs/Src/Platform/EventQueue.cpp
	Code/Libs/Src/Platform/LegacyPreferenceValues.cpp
	Code/Libs/Src/Platform/NativeUserSettings.cpp
	Code/Libs/Src/Platform/NativeUserSettingsOwner.cpp
	Code/Libs/Src/Platform/PreferenceStore.cpp
	Code/Libs/Src/Platform/SettingsPath.cpp
	Code/Libs/Src/Platform/StartupDataRoot.cpp
	Code/Libs/Src/Platform/UtfMetadata.cpp
	Code/Libs/Src/Platform/PlatformTime.cpp
)

add_library(frontierx_math STATIC
	Code/Libs/Include/FrontierX/MathCore.h
	Code/Libs/Src/Platform/MathCore.cpp
)

target_include_directories(frontierx_math
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Include
)

target_compile_features(frontierx_math PUBLIC cxx_std_17)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(frontierx_math PRIVATE -ffp-contract=off)
endif()
frontierx_enable_strict_warnings(frontierx_math)

target_include_directories(frontierx_platform
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Include
)

target_compile_features(frontierx_platform PUBLIC cxx_std_17)
frontierx_enable_strict_warnings(frontierx_platform)
target_link_libraries(frontierx_platform PUBLIC frontierx_profile_document)

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

add_executable(frontierx_event_queue_probe
	Tools/portability/event_queue_probe.cpp
)

target_link_libraries(frontierx_event_queue_probe PRIVATE frontierx_platform)
target_compile_features(frontierx_event_queue_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_event_queue_probe)

add_library(frontierx_dacom_core STATIC
	Code/Libs/Include/FrontierX/NativeComponentModule.h
	Code/Libs/Include/FrontierX/NativeComponentManager.h
	Code/Libs/Src/Platform/NativeComponentManager.cpp
)

target_include_directories(frontierx_dacom_core
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Include
)

target_compile_features(frontierx_dacom_core PUBLIC cxx_std_17)
frontierx_enable_strict_warnings(frontierx_dacom_core)

add_library(frontierx_dynamic_library STATIC
	Code/Libs/Include/FrontierX/DynamicLibrary.h
	Code/Libs/Src/Platform/DynamicLibrary.cpp
)

target_include_directories(frontierx_dynamic_library
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Include
)

target_link_libraries(frontierx_dynamic_library PUBLIC ${CMAKE_DL_LIBS})
target_compile_features(frontierx_dynamic_library PUBLIC cxx_std_17)
frontierx_enable_strict_warnings(frontierx_dynamic_library)

add_library(frontierx_dynamic_library_fixture SHARED
	Tools/portability/dynamic_library_fixture.cpp
)

target_include_directories(frontierx_dynamic_library_fixture
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Include)
target_compile_features(frontierx_dynamic_library_fixture PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_dynamic_library_fixture)

add_executable(frontierx_dynamic_library_probe
	Tools/portability/dynamic_library_probe.cpp
)

target_link_libraries(frontierx_dacom_core
	PUBLIC frontierx_dynamic_library frontierx_profile_document)
target_link_libraries(frontierx_dynamic_library_probe
	PRIVATE frontierx_dynamic_library frontierx_dacom_core)
target_compile_features(frontierx_dynamic_library_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_dynamic_library_probe)
add_dependencies(frontierx_dynamic_library_probe frontierx_dynamic_library_fixture)

add_library(frontierx_event_system STATIC
	Code/Libs/Src/DAHOTKEY/EventSys.cpp
)

target_include_directories(frontierx_event_system
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Include
)

target_compile_features(frontierx_event_system PUBLIC cxx_std_17)
frontierx_enable_strict_warnings(frontierx_event_system)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(frontierx_event_system PRIVATE -Wno-delete-non-virtual-dtor)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_compile_options(frontierx_event_system PRIVATE
		-Wno-delete-non-abstract-non-virtual-dtor
		-Wno-delete-abstract-non-virtual-dtor)
endif()

add_executable(frontierx_event_system_probe
	Tools/portability/event_system_probe.cpp
)

target_link_libraries(frontierx_event_system_probe
	PRIVATE
		frontierx_dacom_core
		frontierx_event_system
)
target_compile_features(frontierx_event_system_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_event_system_probe)

add_library(frontierx_profile_parser STATIC
	Code/Libs/Src/DACOM/ProfileParser.cpp
)

target_include_directories(frontierx_profile_parser
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Include
)
target_compile_features(frontierx_profile_parser PUBLIC cxx_std_17)
frontierx_enable_strict_warnings(frontierx_profile_parser)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(frontierx_profile_parser PRIVATE -Wno-delete-non-virtual-dtor)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_compile_options(frontierx_profile_parser PRIVATE
		-Wno-delete-non-abstract-non-virtual-dtor
		-Wno-delete-abstract-non-virtual-dtor)
endif()

add_executable(frontierx_profile_parser_probe
	Tools/portability/profile_parser_probe.cpp
)
target_link_libraries(frontierx_profile_parser_probe
	PRIVATE frontierx_dacom_core frontierx_profile_parser)
target_compile_features(frontierx_profile_parser_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_profile_parser_probe)

add_executable(frontierx_preference_store_probe
	Tools/portability/preference_store_probe.cpp
)

target_link_libraries(frontierx_preference_store_probe PRIVATE frontierx_platform)
target_compile_features(frontierx_preference_store_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_preference_store_probe)

add_executable(frontierx_native_user_settings_probe
	Tools/portability/native_user_settings_probe.cpp
)

target_link_libraries(frontierx_native_user_settings_probe PRIVATE frontierx_platform)
target_compile_features(frontierx_native_user_settings_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_native_user_settings_probe)

add_executable(frontierx_encoded_handle_probe
	Tools/portability/encoded_handle_probe.cpp
)

target_include_directories(frontierx_encoded_handle_probe PRIVATE Code/Libs/Include)
target_compile_features(frontierx_encoded_handle_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_encoded_handle_probe)

add_executable(frontierx_dacom_runtime_probe
	Tools/portability/dacom_runtime_probe.cpp
)

target_include_directories(frontierx_dacom_runtime_probe PRIVATE Code/Libs/Include)
target_compile_features(frontierx_dacom_runtime_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_dacom_runtime_probe)

add_executable(frontierx_dacom_interface_probe
	Tools/portability/dacom_interface_probe.cpp
)

target_include_directories(frontierx_dacom_interface_probe
	PRIVATE
		Code/App/Src
		Code/Libs/Include
)
target_compile_features(frontierx_dacom_interface_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_dacom_interface_probe)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	# IDAComponent deliberately has no virtual destructor; changing it would break its vtable ABI.
	target_compile_options(frontierx_dacom_interface_probe PRIVATE -Wno-delete-non-virtual-dtor)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_compile_options(frontierx_dacom_interface_probe PRIVATE
		-Wno-delete-non-abstract-non-virtual-dtor)
endif()

add_executable(frontierx_stack_address_probe
	Tools/portability/stack_address_probe.cpp
)

target_include_directories(frontierx_stack_address_probe PRIVATE Code/Libs/Include)
target_compile_features(frontierx_stack_address_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_stack_address_probe)

add_executable(frontierx_particle_context_probe
	Tools/portability/particle_context_probe.cpp
)

target_include_directories(frontierx_particle_context_probe PRIVATE Code/Libs/Include)
target_compile_features(frontierx_particle_context_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_particle_context_probe)

add_executable(frontierx_math_core_probe
	Tools/portability/math_core_probe.cpp
)

target_link_libraries(frontierx_math_core_probe PRIVATE frontierx_math)
target_compile_features(frontierx_math_core_probe PRIVATE cxx_std_17)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(frontierx_math_core_probe PRIVATE -ffp-contract=off)
endif()
frontierx_enable_strict_warnings(frontierx_math_core_probe)

add_executable(frontierx_ray_geometry_probe
	Tools/portability/ray_geometry_probe.cpp
)

target_include_directories(frontierx_ray_geometry_probe PRIVATE Code/Libs/Include)
target_compile_features(frontierx_ray_geometry_probe PRIVATE cxx_std_17)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(frontierx_ray_geometry_probe PRIVATE -ffp-contract=off)
endif()
frontierx_enable_strict_warnings(frontierx_ray_geometry_probe)

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

if(FRONTIERX_BUILD_SDL3)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(SDL3 REQUIRED IMPORTED_TARGET sdl3)
	add_library(frontierx_sdl3_platform STATIC Code/Libs/Src/Platform/SdlWindow.cpp)
	target_sources(frontierx_sdl3_platform PRIVATE Code/Libs/Src/Platform/SdlLegacyInput.cpp)
	target_include_directories(frontierx_sdl3_platform PUBLIC Code/Libs/Include)
	target_link_libraries(frontierx_sdl3_platform PRIVATE PkgConfig::SDL3)
	target_compile_features(frontierx_sdl3_platform PUBLIC cxx_std_17)
	frontierx_enable_strict_warnings(frontierx_sdl3_platform)
	add_library(frontierx_sdl3_event_pump STATIC Code/Libs/Src/Platform/SdlEventPump.cpp)
	target_include_directories(frontierx_sdl3_event_pump PUBLIC Code/Libs/Include)
	target_link_libraries(frontierx_sdl3_event_pump
		PRIVATE frontierx_event_system frontierx_sdl3_platform)
	target_compile_features(frontierx_sdl3_event_pump PUBLIC cxx_std_17)
	frontierx_enable_strict_warnings(frontierx_sdl3_event_pump)
	add_executable(frontierx_native_shell Code/Launcher/NativeShell.cpp)
	target_link_libraries(frontierx_native_shell
		PRIVATE
			frontierx_dacom_core
			frontierx_event_system
			frontierx_platform
			frontierx_sdl3_event_pump
			frontierx_sdl3_platform)
	target_compile_features(frontierx_native_shell PRIVATE cxx_std_17)
	frontierx_enable_strict_warnings(frontierx_native_shell)
	add_executable(frontierx_sdl3_window_probe Tools/portability/sdl3_window_probe.cpp)
	target_link_libraries(frontierx_sdl3_window_probe PRIVATE frontierx_sdl3_platform PkgConfig::SDL3)
	target_compile_features(frontierx_sdl3_window_probe PRIVATE cxx_std_17)
	frontierx_enable_strict_warnings(frontierx_sdl3_window_probe)
	add_executable(frontierx_legacy_input_probe Tools/portability/legacy_input_probe.cpp)
	target_link_libraries(frontierx_legacy_input_probe
		PRIVATE
			frontierx_dacom_core
			frontierx_event_system
			frontierx_sdl3_event_pump
			frontierx_sdl3_platform
			PkgConfig::SDL3)
	target_compile_features(frontierx_legacy_input_probe PRIVATE cxx_std_17)
	frontierx_enable_strict_warnings(frontierx_legacy_input_probe)
endif()

include(CTest)
if(BUILD_TESTING)
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    add_test(NAME portability.headless COMMAND ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability/test_headless.py $<TARGET_FILE:frontierx_headless>)
	add_test(NAME portability.platform_inventory COMMAND ${Python3_EXECUTABLE} -m unittest
		discover -s ${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability
		-p test_platform_inventory.py -v)
	set_tests_properties(portability.platform_inventory PROPERTIES
		ENVIRONMENT "PYTHONDONTWRITEBYTECODE=1")
	add_test(NAME portability.profile_document COMMAND frontierx_profile_document_probe)
	add_test(NAME portability.profile_buffer COMMAND frontierx_profile_buffer_probe)
	add_test(NAME portability.layout COMMAND frontierx_layout_probe)
	add_test(NAME portability.tinyxml COMMAND frontierx_tinyxml_probe)
	add_test(NAME portability.platform_time COMMAND frontierx_platform_time_probe)
	add_test(NAME portability.data_root COMMAND frontierx_data_root_probe --self-test)
	add_test(NAME portability.event_queue COMMAND frontierx_event_queue_probe)
	add_test(NAME portability.dynamic_library COMMAND frontierx_dynamic_library_probe
		$<TARGET_FILE:frontierx_dynamic_library_fixture>)
	add_test(NAME portability.event_system COMMAND frontierx_event_system_probe)
	add_test(NAME portability.profile_parser COMMAND frontierx_profile_parser_probe)
	add_test(NAME portability.preference_store COMMAND frontierx_preference_store_probe)
	add_test(NAME portability.native_user_settings COMMAND frontierx_native_user_settings_probe)
	add_test(NAME portability.encoded_handle COMMAND frontierx_encoded_handle_probe)
	add_test(NAME portability.dacom_runtime COMMAND frontierx_dacom_runtime_probe)
	add_test(NAME portability.dacom_interface COMMAND frontierx_dacom_interface_probe)
	add_test(NAME portability.stack_address COMMAND frontierx_stack_address_probe)
	add_test(NAME portability.particle_context COMMAND frontierx_particle_context_probe)
	add_test(NAME portability.math_core COMMAND frontierx_math_core_probe)
	add_test(NAME portability.ray_geometry COMMAND frontierx_ray_geometry_probe)
	if(TARGET frontierx_sdl3_window_probe)
		add_test(NAME portability.sdl3_window COMMAND frontierx_sdl3_window_probe)
		set_tests_properties(portability.sdl3_window PROPERTIES ENVIRONMENT "SDL_VIDEODRIVER=dummy")
		add_test(NAME portability.legacy_input COMMAND frontierx_legacy_input_probe)
		set_tests_properties(portability.legacy_input PROPERTIES ENVIRONMENT "SDL_VIDEODRIVER=dummy")
		add_test(NAME portability.native_shell COMMAND ${Python3_EXECUTABLE}
			${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability/test_native_shell.py
			$<TARGET_FILE:frontierx_native_shell>)
	endif()
endif()
