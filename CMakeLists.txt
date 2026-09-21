cmake_minimum_required(VERSION 3.25)

project(FrontierX LANGUAGES CXX)

option(FRONTIERX_BUILD_SDL3 "Build the isolated SDL3 window/input backend and probe" OFF)
option(FRONTIERX_BUILD_SDL3_GPU "Build the isolated SDL3 GPU rendering probe" OFF)

if(FRONTIERX_BUILD_SDL3_GPU AND NOT FRONTIERX_BUILD_SDL3)
	message(FATAL_ERROR "FRONTIERX_BUILD_SDL3_GPU requires FRONTIERX_BUILD_SDL3")
endif()

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
	Code/Libs/Include/FrontierX/NativeUtfFileSystem.h
	Code/Libs/Include/FrontierX/PlatformTime.h
	Code/Libs/Include/FrontierX/PreferenceStore.h
	Code/Libs/Include/FrontierX/SettingsPath.h
	Code/Libs/Include/FrontierX/StartupDataRoot.h
	Code/Libs/Include/FrontierX/StartupPreferences.h
	Code/Libs/Src/Platform/DataRoot.cpp
	Code/Libs/Src/Platform/EventQueue.cpp
	Code/Libs/Src/Platform/LegacyPreferenceValues.cpp
	Code/Libs/Src/Platform/NativeUserSettings.cpp
	Code/Libs/Src/Platform/NativeUserSettingsOwner.cpp
	Code/Libs/Src/Platform/NativeUtfFileSystem.cpp
	Code/Libs/Src/Platform/PreferenceStore.cpp
	Code/Libs/Src/Platform/SettingsPath.cpp
	Code/Libs/Src/Platform/StartupDataRoot.cpp
	Code/Libs/Src/Platform/StartupPreferences.cpp
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

add_library(frontierx_native_user_defaults STATIC
	Code/Libs/Include/FrontierX/NativeUserDefaults.h
	Code/Libs/Src/Platform/NativeUserDefaults.cpp
)
target_include_directories(frontierx_native_user_defaults
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Include
		${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src)
target_link_libraries(frontierx_native_user_defaults
	PUBLIC frontierx_dacom_core frontierx_platform)
target_compile_features(frontierx_native_user_defaults PUBLIC cxx_std_17)
frontierx_enable_strict_warnings(frontierx_native_user_defaults)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(frontierx_native_user_defaults PRIVATE
		-Wno-delete-non-virtual-dtor)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_compile_options(frontierx_native_user_defaults PRIVATE
		-Wno-delete-non-abstract-non-virtual-dtor
		-Wno-delete-abstract-non-virtual-dtor)
endif()

add_executable(frontierx_native_user_defaults_probe
	Tools/portability/native_user_defaults_probe.cpp)
target_link_libraries(frontierx_native_user_defaults_probe
	PRIVATE frontierx_native_user_defaults)
target_compile_features(frontierx_native_user_defaults_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_native_user_defaults_probe)

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
	PRIVATE frontierx_dynamic_library frontierx_dacom_core frontierx_native_dacom_bootstrap)
target_compile_features(frontierx_dynamic_library_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_dynamic_library_probe)
set_property(TARGET frontierx_dynamic_library_probe PROPERTY ENABLE_EXPORTS ON)
add_dependencies(frontierx_dynamic_library_probe frontierx_dynamic_library_fixture)

add_library(frontierx_event_system STATIC
	Code/Libs/Src/DAHOTKEY/EventSys.cpp
)

target_include_directories(frontierx_event_system
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Include
)

target_link_libraries(frontierx_event_system PUBLIC frontierx_native_heap)
target_compile_definitions(frontierx_event_system PRIVATE DA_HEAP_ENABLED=1)
target_compile_features(frontierx_event_system PUBLIC cxx_std_17)
frontierx_enable_strict_warnings(frontierx_event_system)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(frontierx_event_system PRIVATE -Wno-delete-non-virtual-dtor)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_compile_options(frontierx_event_system PRIVATE
		-Wno-delete-non-abstract-non-virtual-dtor
		-Wno-delete-abstract-non-virtual-dtor)
endif()

add_library(frontierx_event_system_module SHARED
	Code/Libs/Src/DAHOTKEY/EventSys.cpp
	Code/Libs/Src/Platform/EventSystemModule.cpp
)
set(frontierx_event_system_module_exports
	${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Src/Platform/EventSystemModule.map)
set_property(TARGET frontierx_event_system_module APPEND PROPERTY
	LINK_DEPENDS ${frontierx_event_system_module_exports})
target_link_options(frontierx_event_system_module PRIVATE
	"LINKER:--version-script=${frontierx_event_system_module_exports}")
target_include_directories(frontierx_event_system_module
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Include)
target_compile_definitions(frontierx_event_system_module
	PRIVATE FRONTIERX_EVENT_SYSTEM_MODULE=1 DA_HEAP_ENABLED=1)
target_compile_features(frontierx_event_system_module PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_event_system_module)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(frontierx_event_system_module PRIVATE -Wno-delete-non-virtual-dtor)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_compile_options(frontierx_event_system_module PRIVATE
		-Wno-delete-non-abstract-non-virtual-dtor
		-Wno-delete-abstract-non-virtual-dtor)
endif()
add_dependencies(frontierx_dynamic_library_probe frontierx_event_system_module)

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

add_library(frontierx_hotkey_event STATIC
	Code/Libs/Src/DAHOTKEY/HKEvent.cpp
)
target_include_directories(frontierx_hotkey_event
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Include
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Src/DAHOTKEY)
target_link_libraries(frontierx_hotkey_event PUBLIC frontierx_native_heap)
target_compile_definitions(frontierx_hotkey_event PRIVATE DA_HEAP_ENABLED=1)
target_compile_features(frontierx_hotkey_event PUBLIC cxx_std_17)
frontierx_enable_strict_warnings(frontierx_hotkey_event)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(frontierx_hotkey_event PRIVATE -Wno-delete-non-virtual-dtor)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_compile_options(frontierx_hotkey_event PRIVATE
		-Wno-delete-non-abstract-non-virtual-dtor
		-Wno-delete-abstract-non-virtual-dtor)
endif()

add_executable(frontierx_hotkey_event_probe
	Tools/portability/hotkey_event_probe.cpp
)
target_link_libraries(frontierx_hotkey_event_probe
	PRIVATE
		frontierx_dacom_core
		frontierx_event_system
		frontierx_hotkey_event
		frontierx_platform
)
target_compile_features(frontierx_hotkey_event_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_hotkey_event_probe)

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

add_library(frontierx_native_heap STATIC
	Code/Libs/Include/FrontierX/NativeHeap.h
	Code/Libs/Src/Platform/NativeHeap.cpp)
target_include_directories(frontierx_native_heap
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Include)
target_link_libraries(frontierx_native_heap PUBLIC frontierx_dacom_core)
target_compile_features(frontierx_native_heap PUBLIC cxx_std_17)
frontierx_enable_strict_warnings(frontierx_native_heap)

add_library(frontierx_native_dacom_bootstrap STATIC
	Code/Libs/Src/Platform/NativeDacomBootstrap.cpp)
target_include_directories(frontierx_native_dacom_bootstrap
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Include)
target_link_libraries(frontierx_native_dacom_bootstrap
	PUBLIC frontierx_dacom_core frontierx_native_heap frontierx_profile_parser)
target_compile_features(frontierx_native_dacom_bootstrap PUBLIC cxx_std_17)
frontierx_enable_strict_warnings(frontierx_native_dacom_bootstrap)

add_executable(frontierx_native_dacom_bootstrap_probe
	Tools/portability/native_dacom_bootstrap_probe.cpp)
target_link_libraries(frontierx_native_dacom_bootstrap_probe
	PRIVATE frontierx_native_dacom_bootstrap)
target_compile_features(frontierx_native_dacom_bootstrap_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_native_dacom_bootstrap_probe)

add_executable(frontierx_native_heap_probe
	Tools/portability/native_heap_probe.cpp)
target_link_libraries(frontierx_native_heap_probe
	PRIVATE frontierx_native_dacom_bootstrap)
target_compile_features(frontierx_native_heap_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_native_heap_probe)

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

add_library(frontierx_render_submission INTERFACE)
target_include_directories(frontierx_render_submission INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Include)
target_compile_features(frontierx_render_submission INTERFACE cxx_std_17)
add_library(frontierx_native_render_primitive STATIC
	Code/Libs/Include/FrontierX/NativeRenderPrimitive.h
	Code/Libs/Src/Platform/NativeRenderPrimitive.cpp)
target_include_directories(frontierx_native_render_primitive PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Include)
target_link_libraries(frontierx_native_render_primitive
	PUBLIC frontierx_dacom_core frontierx_render_submission)
target_compile_features(frontierx_native_render_primitive PUBLIC cxx_std_17)
frontierx_enable_strict_warnings(frontierx_native_render_primitive)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(frontierx_native_render_primitive PRIVATE
		-Wno-delete-non-virtual-dtor)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_compile_options(frontierx_native_render_primitive PRIVATE
		-Wno-delete-non-abstract-non-virtual-dtor
		-Wno-delete-abstract-non-virtual-dtor)
endif()
add_executable(frontierx_render_submission_probe
	Tools/portability/render_submission_probe.cpp)
target_link_libraries(frontierx_render_submission_probe
	PRIVATE frontierx_native_render_primitive)
frontierx_enable_strict_warnings(frontierx_render_submission_probe)

add_library(frontierx_render_smoke_scene STATIC
	Code/Libs/Include/FrontierX/RenderSmokeScene.h
	Code/Libs/Src/Platform/RenderSmokeScene.cpp)
target_include_directories(frontierx_render_smoke_scene PUBLIC Code/Libs/Include)
target_compile_features(frontierx_render_smoke_scene PUBLIC cxx_std_17)
frontierx_enable_strict_warnings(frontierx_render_smoke_scene)

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
	add_library(frontierx_native_window_manager STATIC
		Code/Libs/Include/FrontierX/NativeWindowManager.h
		Code/Libs/Src/Platform/NativeWindowManager.cpp)
	target_include_directories(frontierx_native_window_manager PUBLIC Code/Libs/Include)
	target_link_libraries(frontierx_native_window_manager
		PUBLIC frontierx_sdl3_platform)
	target_compile_features(frontierx_native_window_manager PUBLIC cxx_std_17)
	frontierx_enable_strict_warnings(frontierx_native_window_manager)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		target_compile_options(frontierx_native_window_manager PRIVATE
			-Wno-delete-non-virtual-dtor)
	elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		target_compile_options(frontierx_native_window_manager PRIVATE
			-Wno-delete-non-abstract-non-virtual-dtor
			-Wno-delete-abstract-non-virtual-dtor)
	endif()
	add_executable(frontierx_native_shell Code/Launcher/NativeShell.cpp)
	add_dependencies(frontierx_native_shell frontierx_event_system_module)
	target_link_libraries(frontierx_native_shell
		PRIVATE
			frontierx_dacom_core
			frontierx_event_system
			frontierx_hotkey_event
			frontierx_native_dacom_bootstrap
			frontierx_native_user_defaults
			frontierx_native_window_manager
			frontierx_platform
			PkgConfig::SDL3)
	target_compile_features(frontierx_native_shell PRIVATE cxx_std_17)
	frontierx_enable_strict_warnings(frontierx_native_shell)
	set_property(TARGET frontierx_native_shell PROPERTY ENABLE_EXPORTS ON)
	add_executable(frontierx_sdl3_window_probe Tools/portability/sdl3_window_probe.cpp)
	target_link_libraries(frontierx_sdl3_window_probe PRIVATE frontierx_sdl3_platform PkgConfig::SDL3)
	target_compile_features(frontierx_sdl3_window_probe PRIVATE cxx_std_17)
	frontierx_enable_strict_warnings(frontierx_sdl3_window_probe)
	if(FRONTIERX_BUILD_SDL3_GPU)
		find_program(FRONTIERX_GLSLC_EXECUTABLE NAMES glslc REQUIRED)
		add_library(frontierx_sdl3_gpu_renderer STATIC
			Code/Libs/Include/FrontierX/SdlGpuRenderer.h
			Code/Libs/Src/Platform/SdlGpuRenderer.cpp)
		target_include_directories(frontierx_sdl3_gpu_renderer PUBLIC Code/Libs/Include)
		target_link_libraries(frontierx_sdl3_gpu_renderer
			PUBLIC frontierx_render_submission PRIVATE PkgConfig::SDL3)
		target_compile_features(frontierx_sdl3_gpu_renderer PUBLIC cxx_std_17)
		frontierx_enable_strict_warnings(frontierx_sdl3_gpu_renderer)
		target_link_libraries(frontierx_native_shell PRIVATE
			frontierx_native_render_primitive
			frontierx_render_smoke_scene
			frontierx_sdl3_gpu_renderer)
		target_compile_definitions(frontierx_native_shell PRIVATE
			FRONTIERX_HAS_SDL_GPU_RENDERER=1)
		set(FRONTIERX_GPU_SHADER_SOURCE_DIR
			${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability/shaders)
		set(FRONTIERX_GPU_SHADER_BINARY_DIR
			${CMAKE_CURRENT_BINARY_DIR}/generated/shaders)
		set(FRONTIERX_GPU_VERTEX_SHADER
			${FRONTIERX_GPU_SHADER_BINARY_DIR}/sdl3_gpu_probe.vert.spv)
		set(FRONTIERX_GPU_FRAGMENT_SHADER
			${FRONTIERX_GPU_SHADER_BINARY_DIR}/sdl3_gpu_probe.frag.spv)
		add_custom_command(
			OUTPUT ${FRONTIERX_GPU_VERTEX_SHADER}
			COMMAND ${CMAKE_COMMAND} -E make_directory ${FRONTIERX_GPU_SHADER_BINARY_DIR}
			COMMAND ${FRONTIERX_GLSLC_EXECUTABLE}
				--target-env=vulkan1.0
				-fshader-stage=vert
				-o ${FRONTIERX_GPU_VERTEX_SHADER}
				${FRONTIERX_GPU_SHADER_SOURCE_DIR}/sdl3_gpu_probe.vert
			DEPENDS ${FRONTIERX_GPU_SHADER_SOURCE_DIR}/sdl3_gpu_probe.vert
			VERBATIM)
		add_custom_command(
			OUTPUT ${FRONTIERX_GPU_FRAGMENT_SHADER}
			COMMAND ${CMAKE_COMMAND} -E make_directory ${FRONTIERX_GPU_SHADER_BINARY_DIR}
			COMMAND ${FRONTIERX_GLSLC_EXECUTABLE}
				--target-env=vulkan1.0
				-fshader-stage=frag
				-o ${FRONTIERX_GPU_FRAGMENT_SHADER}
				${FRONTIERX_GPU_SHADER_SOURCE_DIR}/sdl3_gpu_probe.frag
			DEPENDS ${FRONTIERX_GPU_SHADER_SOURCE_DIR}/sdl3_gpu_probe.frag
			VERBATIM)
		add_custom_target(frontierx_sdl3_gpu_shaders DEPENDS
			${FRONTIERX_GPU_VERTEX_SHADER}
			${FRONTIERX_GPU_FRAGMENT_SHADER})
		add_executable(frontierx_sdl3_gpu_probe Tools/portability/sdl3_gpu_probe.cpp)
		add_dependencies(frontierx_sdl3_gpu_probe frontierx_sdl3_gpu_shaders)
		target_link_libraries(frontierx_sdl3_gpu_probe
			PRIVATE
				frontierx_native_render_primitive
				frontierx_render_smoke_scene
				frontierx_sdl3_gpu_renderer
				PkgConfig::SDL3)
		target_compile_features(frontierx_sdl3_gpu_probe PRIVATE cxx_std_17)
		frontierx_enable_strict_warnings(frontierx_sdl3_gpu_probe)
	endif()
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
	add_executable(frontierx_native_window_manager_probe
		Tools/portability/native_window_manager_probe.cpp)
	target_link_libraries(frontierx_native_window_manager_probe
		PRIVATE
			frontierx_dacom_core
			frontierx_native_window_manager
			PkgConfig::SDL3)
	target_compile_features(frontierx_native_window_manager_probe PRIVATE cxx_std_17)
	frontierx_enable_strict_warnings(frontierx_native_window_manager_probe)
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
		$<TARGET_FILE:frontierx_dynamic_library_fixture>
		$<TARGET_FILE:frontierx_event_system_module>)
	add_test(NAME portability.event_system COMMAND frontierx_event_system_probe)
	add_test(NAME portability.hotkey_event COMMAND frontierx_hotkey_event_probe
		${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/Hotkeys.utf
		${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/DBHotkeys.utf)
	add_test(NAME portability.profile_parser COMMAND frontierx_profile_parser_probe)
	add_test(NAME portability.native_dacom_bootstrap COMMAND frontierx_native_dacom_bootstrap_probe)
	add_test(NAME portability.native_heap COMMAND frontierx_native_heap_probe)
	add_test(NAME portability.preference_store COMMAND frontierx_preference_store_probe)
	add_test(NAME portability.native_user_settings COMMAND frontierx_native_user_settings_probe)
	add_test(NAME portability.native_user_defaults COMMAND frontierx_native_user_defaults_probe)
	add_test(NAME portability.encoded_handle COMMAND frontierx_encoded_handle_probe)
	add_test(NAME portability.dacom_runtime COMMAND frontierx_dacom_runtime_probe)
	add_test(NAME portability.dacom_interface COMMAND frontierx_dacom_interface_probe)
	add_test(NAME portability.stack_address COMMAND frontierx_stack_address_probe)
	add_test(NAME portability.particle_context COMMAND frontierx_particle_context_probe)
	add_test(NAME portability.math_core COMMAND frontierx_math_core_probe)
	add_test(NAME portability.ray_geometry COMMAND frontierx_ray_geometry_probe)
	add_test(NAME portability.render_submission COMMAND frontierx_render_submission_probe)
	if(TARGET frontierx_sdl3_window_probe)
		add_test(NAME portability.sdl3_window COMMAND frontierx_sdl3_window_probe)
		set_tests_properties(portability.sdl3_window PROPERTIES ENVIRONMENT "SDL_VIDEODRIVER=dummy")
		add_test(NAME portability.legacy_input COMMAND frontierx_legacy_input_probe)
		set_tests_properties(portability.legacy_input PROPERTIES ENVIRONMENT "SDL_VIDEODRIVER=dummy")
		add_test(NAME portability.native_window_manager COMMAND frontierx_native_window_manager_probe)
		set_tests_properties(portability.native_window_manager PROPERTIES
			ENVIRONMENT "SDL_VIDEODRIVER=dummy")
		add_test(NAME portability.native_shell COMMAND ${Python3_EXECUTABLE}
			${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability/test_native_shell.py
			$<TARGET_FILE:frontierx_native_shell>
			$<TARGET_FILE:frontierx_event_system_module>
			${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/Hotkeys.utf)
	endif()
	if(TARGET frontierx_sdl3_gpu_probe)
		add_test(NAME portability.sdl3_gpu COMMAND frontierx_sdl3_gpu_probe
			${FRONTIERX_GPU_VERTEX_SHADER}
			${FRONTIERX_GPU_FRAGMENT_SHADER}
			--visible)
		set_tests_properties(portability.sdl3_gpu PROPERTIES SKIP_RETURN_CODE 77)
	endif()
endif()
