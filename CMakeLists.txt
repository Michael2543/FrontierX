cmake_minimum_required(VERSION 3.25)

project(FrontierX LANGUAGES CXX)

option(FRONTIERX_BUILD_SDL3 "Build the isolated SDL3 window/input backend and probe" OFF)
option(FRONTIERX_BUILD_FFMPEG_AUDIO "Enable the external shared LGPL FFmpeg IAC decoder" OFF)
option(FRONTIERX_BUILD_FFMPEG_VIDEO "Enable the external shared LGPL FFmpeg movie owner" OFF)
option(FRONTIERX_BUILD_SDL3_GPU "Build the isolated SDL3 GPU rendering probe" OFF)

if(FRONTIERX_BUILD_FFMPEG_AUDIO AND NOT FRONTIERX_BUILD_SDL3)
	message(FATAL_ERROR "FRONTIERX_BUILD_FFMPEG_AUDIO requires FRONTIERX_BUILD_SDL3")
endif()
if(FRONTIERX_BUILD_FFMPEG_VIDEO AND NOT FRONTIERX_BUILD_SDL3)
	message(FATAL_ERROR "FRONTIERX_BUILD_FFMPEG_VIDEO requires FRONTIERX_BUILD_SDL3")
endif()

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
		${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src
		${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/Include
		${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/Scripts/Sol_T
		${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Include
)

target_compile_features(frontierx_layout_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_layout_probe)

# Compile the selected source training script and its real helper while the
# production MScript/module loader is brought into the native process link.
add_library(frontierx_sol_t_script OBJECT
	Code/App/Src/Scripts/Sol_T/Sol_T.cpp
	Code/App/Src/Scripts/Helper/helper.cpp
)
target_include_directories(frontierx_sol_t_script PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/Code/App/DInclude
	${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src
	${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/Include
	${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/Scripts/Include
	${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/Scripts/Sol_T
	${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Include
)
target_compile_features(frontierx_sol_t_script PRIVATE cxx_std_17)
set_target_properties(frontierx_sol_t_script PROPERTIES POSITION_INDEPENDENT_CODE ON)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(frontierx_sol_t_script PRIVATE
		-Wno-unknown-pragmas -Wno-write-strings)
endif()
set(frontierx_sol_t_schema ${CMAKE_CURRENT_BINARY_DIR}/Sol_T_DataDef.i)
add_custom_command(OUTPUT ${frontierx_sol_t_schema}
	COMMAND ${CMAKE_CXX_COMPILER} -E -P -D_ADB
		-MD -MF ${frontierx_sol_t_schema}.d
		-I${CMAKE_CURRENT_SOURCE_DIR}/Code/App/DInclude
		-I${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src
		-I${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/Include
		-I${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/Scripts/Include
		-I${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/Scripts/Sol_T
		-I${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Include
		${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/Scripts/Sol_T/DataDef.cpp
		-o ${frontierx_sol_t_schema}
	DEPENDS
		Code/App/Src/Scripts/Sol_T/DataDef.cpp
		Code/App/Src/Scripts/Sol_T/DataDef.h
	DEPFILE ${frontierx_sol_t_schema}.d
	VERBATIM
)
add_custom_target(frontierx_sol_t_schema ALL DEPENDS ${frontierx_sol_t_schema})
find_package(Python3 REQUIRED COMPONENTS Interpreter)
find_program(FRONTIERX_LLVM_RC NAMES llvm-rc REQUIRED)
set(frontierx_app_string_rc ${CMAKE_CURRENT_BINARY_DIR}/generated/AppStrings.rc)
set(frontierx_app_string_res ${CMAKE_CURRENT_BINARY_DIR}/generated/AppStrings.res)
set(frontierx_app_string_include ${CMAKE_CURRENT_BINARY_DIR}/generated/NativeAppStrings.inc)
add_custom_command(OUTPUT ${frontierx_app_string_rc}
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/generated
	COMMAND ${Python3_EXECUTABLE}
		${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability/embed_app_strings.py
		--rc ${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/Globals.rc
		${frontierx_app_string_rc}
	DEPENDS Code/App/Src/Globals.rc Tools/portability/embed_app_strings.py
	VERBATIM)
add_custom_command(OUTPUT ${frontierx_app_string_res}
	COMMAND ${FRONTIERX_LLVM_RC} /C 1252 /FO ${frontierx_app_string_res}
		/I ${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src
		/I ${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability/sol_t_rc_include
		${frontierx_app_string_rc}
	DEPENDS ${frontierx_app_string_rc} Code/App/Src/resource.h
	VERBATIM)
add_custom_command(OUTPUT ${frontierx_app_string_include}
	COMMAND ${Python3_EXECUTABLE}
		${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability/embed_app_strings.py
		--include ${frontierx_app_string_res} ${frontierx_app_string_include}
	DEPENDS ${frontierx_app_string_res} Tools/portability/embed_app_strings.py
		Tools/portability/embed_sol_t_strings.py
	VERBATIM)
add_library(frontierx_app_string_resource STATIC
	Code/App/Src/NativeAppStrings.cpp ${frontierx_app_string_include})
target_include_directories(frontierx_app_string_resource PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}/generated Code/App/Src Code/App/Src/Include
	Code/Libs/Include)
target_compile_features(frontierx_app_string_resource PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_app_string_resource)
add_library(frontierx_native_search STATIC Code/App/Src/NativeSearch.cpp)
target_include_directories(frontierx_native_search PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude Code/Libs/Include)
target_compile_features(frontierx_native_search PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_native_search)
add_library(frontierx_native_in_progress_anim STATIC
	Code/App/Src/NativeInProgressAnim.cpp)
target_include_directories(frontierx_native_in_progress_anim PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude Code/Libs/Include)
target_compile_features(frontierx_native_in_progress_anim PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_native_in_progress_anim)
set(frontierx_app_version_include
	${CMAKE_CURRENT_BINARY_DIR}/generated/NativeAppVersion.inc)
add_custom_command(OUTPUT ${frontierx_app_version_include}
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/generated
	COMMAND ${Python3_EXECUTABLE}
		${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability/embed_app_version.py
		${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/Conquest.rc
		${frontierx_app_version_include}
	DEPENDS Code/App/Src/Conquest.rc Tools/portability/embed_app_version.py
	VERBATIM)
add_library(frontierx_native_app_version STATIC
	Code/App/Src/NativeAppVersion.cpp ${frontierx_app_version_include})
target_include_directories(frontierx_native_app_version PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}/generated Code/App/Src Code/App/Src/Include
	Code/App/DInclude Code/Libs/Include)
target_compile_features(frontierx_native_app_version PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_native_app_version)
add_library(frontierx_native_app_text_resources STATIC
	Code/App/Src/NativeAppTextResources.cpp)
target_include_directories(frontierx_native_app_text_resources PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude Code/Libs/Include)
target_compile_features(frontierx_native_app_text_resources PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_native_app_text_resources)
if(FRONTIERX_BUILD_SDL3_GPU)
	target_compile_definitions(frontierx_native_app_text_resources PRIVATE FRONTIERX_NATIVE_SOURCE_TEXT=1)
endif()
set(frontierx_sol_t_resource_dir ${CMAKE_CURRENT_BINARY_DIR}/generated/sol_t)
set(frontierx_sol_t_rc_resource ${frontierx_sol_t_resource_dir}/Sol_T.res)
add_custom_command(OUTPUT ${frontierx_sol_t_rc_resource}
	COMMAND ${CMAKE_COMMAND} -E make_directory ${frontierx_sol_t_resource_dir}
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${frontierx_sol_t_schema} ${frontierx_sol_t_resource_dir}/DataDef.i
	COMMAND ${FRONTIERX_LLVM_RC}
		/FO ${frontierx_sol_t_rc_resource}
		/I ${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability/sol_t_rc_include
		/I ${frontierx_sol_t_resource_dir}
		/I ${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/Scripts/Sol_T
		${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/Scripts/Sol_T/Sol_T.rc
	DEPENDS ${frontierx_sol_t_schema}
		Code/App/Src/Scripts/Sol_T/Sol_T.rc
		Code/App/Src/Scripts/Sol_T/resource.h
		Tools/portability/sol_t_rc_include/winres.h
	VERBATIM)
set(frontierx_sol_t_strings_include
	${CMAKE_CURRENT_BINARY_DIR}/generated/NativeSolTrainStrings.inc)
add_custom_command(OUTPUT ${frontierx_sol_t_strings_include}
	COMMAND ${Python3_EXECUTABLE}
		${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability/embed_sol_t_strings.py
		${frontierx_sol_t_rc_resource} ${frontierx_sol_t_strings_include}
	DEPENDS ${frontierx_sol_t_rc_resource}
		Tools/portability/embed_sol_t_strings.py
	VERBATIM)
add_library(frontierx_sol_t_string_resource STATIC
	Code/App/Src/Scripts/Sol_T/NativeSolTrainStrings.cpp
	${frontierx_sol_t_strings_include})
target_include_directories(frontierx_sol_t_string_resource PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}/generated)
target_compile_features(frontierx_sol_t_string_resource PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_sol_t_string_resource)
target_link_libraries(frontierx_sol_t_script PUBLIC frontierx_sol_t_string_resource)
add_library(frontierx_app_script_owner OBJECT
	Code/App/Src/MScript.cpp)
target_link_libraries(frontierx_app_script_owner PUBLIC
	frontierx_sol_t_string_resource)
target_include_directories(frontierx_app_script_owner PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/Code/App/DInclude
	${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src
	${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/Include
	${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Include
	${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Src/Viewer/DOCUVIEW)
target_compile_features(frontierx_app_script_owner PRIVATE cxx_std_17)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(frontierx_app_script_owner PRIVATE -Wno-unknown-pragmas)
endif()
add_library(frontierx_app_scripting_owner OBJECT
	Code/App/Src/scripting.cpp)
target_include_directories(frontierx_app_scripting_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_scripting_owner PRIVATE cxx_std_17)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(frontierx_app_scripting_owner PRIVATE -ffp-contract=off)
endif()
set(frontierx_sol_t_schema_include
	${CMAKE_CURRENT_BINARY_DIR}/generated/NativeSolTrainSchema.inc)
add_custom_command(OUTPUT ${frontierx_sol_t_schema_include}
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/generated
	COMMAND ${Python3_EXECUTABLE}
		${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability/embed_app_schema.py
		${frontierx_sol_t_schema} ${frontierx_sol_t_schema_include}
		solTrainSchemaBytes
	DEPENDS ${frontierx_sol_t_schema}
		${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability/embed_app_schema.py
	VERBATIM)
add_library(frontierx_sol_t_schema_resource STATIC
	Code/App/Src/Scripts/Sol_T/NativeSolTrainSchema.cpp
	${frontierx_sol_t_schema_include})
target_include_directories(frontierx_sol_t_schema_resource PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}/generated)
target_compile_features(frontierx_sol_t_schema_resource PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_sol_t_schema_resource)
target_link_libraries(frontierx_sol_t_script PUBLIC frontierx_sol_t_schema_resource)

# First regular Terran campaign: original Script01T owner/schema/RT_STRING.
set(frontierx_script01_dir ${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/Scripts/Script02T/Script01T)
set(frontierx_script01_schema ${CMAKE_CURRENT_BINARY_DIR}/Script01T_DataDef.i)
add_custom_command(OUTPUT ${frontierx_script01_schema}
    COMMAND ${CMAKE_CXX_COMPILER} -E -P -D_ADB -MD -MF ${frontierx_script01_schema}.d
        -I${CMAKE_CURRENT_SOURCE_DIR}/Code/App/DInclude
        -I${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/Include
        -I${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Include
        ${frontierx_script01_dir}/DataDef.cpp -o ${frontierx_script01_schema}
    DEPENDS ${frontierx_script01_dir}/DataDef.cpp ${frontierx_script01_dir}/DataDef.h
    DEPFILE ${frontierx_script01_schema}.d VERBATIM)
set(frontierx_script01_resdir ${CMAKE_CURRENT_BINARY_DIR}/generated/script01)
set(frontierx_script01_res ${frontierx_script01_resdir}/Script01T.res)
add_custom_command(OUTPUT ${frontierx_script01_res}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${frontierx_script01_resdir}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${frontierx_script01_schema} ${frontierx_script01_resdir}/DataDef.i
    COMMAND ${FRONTIERX_LLVM_RC} /C 1252 /FO ${frontierx_script01_res}
        /I ${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability/sol_t_rc_include
        /I ${frontierx_script01_resdir} /I ${frontierx_script01_dir}
        ${frontierx_script01_dir}/Script01T.rc
    DEPENDS ${frontierx_script01_schema} ${frontierx_script01_dir}/Script01T.rc
        ${frontierx_script01_dir}/resource.h Tools/portability/sol_t_rc_include/winres.h VERBATIM)
set(frontierx_script01_schema_inc ${CMAKE_CURRENT_BINARY_DIR}/generated/NativeScript01Schema.inc)
set(frontierx_script01_strings_inc ${CMAKE_CURRENT_BINARY_DIR}/generated/NativeScript01Strings.inc)
add_custom_command(OUTPUT ${frontierx_script01_schema_inc}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability/embed_app_schema.py
        ${frontierx_script01_schema} ${frontierx_script01_schema_inc} script01SchemaBytes
    DEPENDS ${frontierx_script01_schema} Tools/portability/embed_app_schema.py VERBATIM)
add_custom_command(OUTPUT ${frontierx_script01_strings_inc}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability/embed_sol_t_strings.py
        ${frontierx_script01_res} ${frontierx_script01_strings_inc}
    DEPENDS ${frontierx_script01_res} Tools/portability/embed_sol_t_strings.py VERBATIM)
add_library(frontierx_script01_resources STATIC
    ${frontierx_script01_dir}/NativeScript01Resources.cpp
    ${frontierx_script01_schema_inc} ${frontierx_script01_strings_inc})
target_include_directories(frontierx_script01_resources PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated)
target_compile_features(frontierx_script01_resources PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_script01_resources)
add_library(frontierx_script01_owner OBJECT ${frontierx_script01_dir}/Script01T.cpp)
target_include_directories(frontierx_script01_owner PRIVATE Code/App/DInclude Code/App/Src
    Code/App/Src/Include Code/App/Src/Scripts/Include Code/Libs/Include)
target_compile_features(frontierx_script01_owner PRIVATE cxx_std_17)
target_compile_definitions(frontierx_script01_owner PRIVATE FRONTIERX_NATIVE_SCRIPT_NAME="Script01T.dll")
target_compile_definitions(frontierx_sol_t_script PRIVATE FRONTIERX_NATIVE_SCRIPT_NAME="Sol_T.dll")
target_link_libraries(frontierx_app_script_owner PUBLIC frontierx_script01_resources)

# Second Terran mission: original Script02T owner/schema/RT_STRING.
set(frontierx_script02_dir ${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/Scripts/Script02T)
set(frontierx_script02_schema ${CMAKE_CURRENT_BINARY_DIR}/Script02T_DataDef.i)
add_custom_command(OUTPUT ${frontierx_script02_schema}
    COMMAND ${CMAKE_CXX_COMPILER} -E -P -D_ADB -MD -MF ${frontierx_script02_schema}.d
        -I${CMAKE_CURRENT_SOURCE_DIR}/Code/App/DInclude
        -I${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/Include
        -I${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Include
        ${frontierx_script02_dir}/DataDef.cpp -o ${frontierx_script02_schema}
    DEPENDS ${frontierx_script02_dir}/DataDef.cpp ${frontierx_script02_dir}/DataDef.h
    DEPFILE ${frontierx_script02_schema}.d VERBATIM)
set(frontierx_script02_resdir ${CMAKE_CURRENT_BINARY_DIR}/generated/script02)
set(frontierx_script02_res ${frontierx_script02_resdir}/Script02T.res)
add_custom_command(OUTPUT ${frontierx_script02_res}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${frontierx_script02_resdir}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${frontierx_script02_schema} ${frontierx_script02_resdir}/DataDef.i
    COMMAND ${FRONTIERX_LLVM_RC} /C 1252 /FO ${frontierx_script02_res}
        /I ${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability/sol_t_rc_include
        /I ${frontierx_script02_resdir} /I ${frontierx_script02_dir}
        ${frontierx_script02_dir}/Script02t.rc
    DEPENDS ${frontierx_script02_schema} ${frontierx_script02_dir}/Script02t.rc
        ${frontierx_script02_dir}/resource.h Tools/portability/sol_t_rc_include/winres.h VERBATIM)
set(frontierx_script02_schema_inc ${CMAKE_CURRENT_BINARY_DIR}/generated/NativeScript02Schema.inc)
set(frontierx_script02_strings_inc ${CMAKE_CURRENT_BINARY_DIR}/generated/NativeScript02Strings.inc)
add_custom_command(OUTPUT ${frontierx_script02_schema_inc}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability/embed_app_schema.py
        ${frontierx_script02_schema} ${frontierx_script02_schema_inc} script02SchemaBytes
    DEPENDS ${frontierx_script02_schema} Tools/portability/embed_app_schema.py VERBATIM)
add_custom_command(OUTPUT ${frontierx_script02_strings_inc}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability/embed_sol_t_strings.py
        ${frontierx_script02_res} ${frontierx_script02_strings_inc}
    DEPENDS ${frontierx_script02_res} Tools/portability/embed_sol_t_strings.py VERBATIM)
add_library(frontierx_script02_resources STATIC
    ${frontierx_script02_dir}/NativeScript02Resources.cpp
    ${frontierx_script02_schema_inc} ${frontierx_script02_strings_inc})
target_include_directories(frontierx_script02_resources PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated)
target_compile_features(frontierx_script02_resources PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_script02_resources)
add_library(frontierx_script02_owner OBJECT ${frontierx_script02_dir}/Script02t.cpp)
target_include_directories(frontierx_script02_owner PRIVATE Code/App/DInclude Code/App/Src
    Code/App/Src/Include Code/App/Src/Scripts/Include Code/Libs/Include)
target_compile_features(frontierx_script02_owner PRIVATE cxx_std_17)
target_compile_definitions(frontierx_script02_owner PRIVATE FRONTIERX_NATIVE_SCRIPT_NAME="Script02T.dll")
target_link_libraries(frontierx_app_script_owner PUBLIC frontierx_script02_resources)

# Third Terran mission: original Script03T owner/schema/RT_STRING.
set(frontierx_script03_dir ${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/Scripts/Script03T)
set(frontierx_script03_schema ${CMAKE_CURRENT_BINARY_DIR}/Script03T_DataDef.i)
add_custom_command(OUTPUT ${frontierx_script03_schema}
    COMMAND ${CMAKE_CXX_COMPILER} -E -P -D_ADB -MD -MF ${frontierx_script03_schema}.d
        -I${CMAKE_CURRENT_SOURCE_DIR}/Code/App/DInclude
        -I${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/Include
        -I${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Include
        ${frontierx_script03_dir}/DataDef.cpp -o ${frontierx_script03_schema}
    DEPENDS ${frontierx_script03_dir}/DataDef.cpp ${frontierx_script03_dir}/DataDef.h
    DEPFILE ${frontierx_script03_schema}.d VERBATIM)
set(frontierx_script03_resdir ${CMAKE_CURRENT_BINARY_DIR}/generated/script03)
set(frontierx_script03_res ${frontierx_script03_resdir}/Script03T.res)
add_custom_command(OUTPUT ${frontierx_script03_res}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${frontierx_script03_resdir}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${frontierx_script03_schema} ${frontierx_script03_resdir}/DataDef.i
    COMMAND ${FRONTIERX_LLVM_RC} /C 1252 /FO ${frontierx_script03_res}
        /I ${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability/sol_t_rc_include
        /I ${frontierx_script03_resdir} /I ${frontierx_script03_dir}
        ${frontierx_script03_dir}/SCRIPT03T.rc
    DEPENDS ${frontierx_script03_schema} ${frontierx_script03_dir}/SCRIPT03T.rc
        ${frontierx_script03_dir}/resource.h Tools/portability/sol_t_rc_include/winres.h VERBATIM)
set(frontierx_script03_schema_inc ${CMAKE_CURRENT_BINARY_DIR}/generated/NativeScript03Schema.inc)
set(frontierx_script03_strings_inc ${CMAKE_CURRENT_BINARY_DIR}/generated/NativeScript03Strings.inc)
add_custom_command(OUTPUT ${frontierx_script03_schema_inc}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability/embed_app_schema.py
        ${frontierx_script03_schema} ${frontierx_script03_schema_inc} script03SchemaBytes
    DEPENDS ${frontierx_script03_schema} Tools/portability/embed_app_schema.py VERBATIM)
add_custom_command(OUTPUT ${frontierx_script03_strings_inc}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability/embed_sol_t_strings.py
        ${frontierx_script03_res} ${frontierx_script03_strings_inc}
    DEPENDS ${frontierx_script03_res} Tools/portability/embed_sol_t_strings.py VERBATIM)
add_library(frontierx_script03_resources STATIC
    ${frontierx_script03_dir}/NativeScript03Resources.cpp
    ${frontierx_script03_schema_inc} ${frontierx_script03_strings_inc})
target_include_directories(frontierx_script03_resources PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated)
target_compile_features(frontierx_script03_resources PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_script03_resources)
add_library(frontierx_script03_owner OBJECT ${frontierx_script03_dir}/SCRIPT03T.cpp)
target_include_directories(frontierx_script03_owner PRIVATE Code/App/DInclude Code/App/Src
    Code/App/Src/Include Code/App/Src/Scripts/Include Code/Libs/Include)
target_compile_features(frontierx_script03_owner PRIVATE cxx_std_17)
target_compile_definitions(frontierx_script03_owner PRIVATE FRONTIERX_NATIVE_SCRIPT_NAME="Script03T.dll")
target_link_libraries(frontierx_app_script_owner PUBLIC frontierx_script03_resources)

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

add_executable(frontierx_string_table_file_text_probe
	Tools/portability/string_table_file_text_probe.cpp
)
target_include_directories(frontierx_string_table_file_text_probe PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Include)
target_compile_features(frontierx_string_table_file_text_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_string_table_file_text_probe)

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
	Code/Libs/Include/FrontierX/NativeWritableUtfArchive.h
	Code/Libs/Include/FrontierX/UtfMetadata.h
	Code/Libs/Include/FrontierX/NativeDiskSpace.h
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
	Code/Libs/Src/Platform/NativeUtfWriter.cpp
	Code/Libs/Src/Platform/NativeWritableUtfArchive.cpp
	Code/Libs/Src/Platform/NativeDiskSpace.cpp
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
target_include_directories(frontierx_data_root_probe PRIVATE Code/App/Src/Include)
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

# Production DOCUVIEW parser core: the game script loader consumes its Win32
# symbol layout through IViewConstructor2. Viewer/window ownership is separate.
add_library(frontierx_view_parser_core STATIC
	Code/Libs/Src/Viewer/DOCUVIEW/SYMTABLE.cpp
	Code/Libs/Src/Viewer/DOCUVIEW/PARS1.cpp
	Code/Libs/Src/Viewer/DOCUVIEW/LXTABLES.cpp
	Code/Libs/Src/Viewer/DOCUVIEW/SCANNER.cpp
	Code/Libs/Src/Viewer/DOCUVIEW/NUMBER.cpp
	Code/Libs/Src/Viewer/DOCUVIEW/PRTTOKEN.cpp
	Code/Libs/Src/Viewer/DOCUVIEW/FERROR.cpp
	Code/Libs/Src/Viewer/DOCUVIEW/FILESTRM.cpp
	Code/Libs/Src/Viewer/DOCUVIEW/DataParser.cpp)
target_include_directories(frontierx_view_parser_core PUBLIC
	Code/Libs/Include Code/Libs/Src/Viewer/DOCUVIEW)
target_link_libraries(frontierx_view_parser_core PUBLIC frontierx_native_heap)
target_compile_features(frontierx_view_parser_core PUBLIC cxx_std_17)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(frontierx_view_parser_core PRIVATE
		-Wall -Wextra -Wpedantic)
endif()

add_library(frontierx_view_constructor STATIC
	Code/Libs/Src/Viewer/DOCUVIEW/ViewCnst.cpp
	Code/Libs/Src/Viewer/DOCUVIEW/ViewSymbolCopy.cpp)
target_link_libraries(frontierx_view_constructor PUBLIC
	frontierx_view_parser_core)
target_compile_features(frontierx_view_constructor PUBLIC cxx_std_17)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(frontierx_view_constructor PRIVATE
		-Wall -Wextra -Wpedantic)
endif()

add_executable(frontierx_view_parser_probe
	Tools/portability/view_parser_probe.cpp)
set(FRONTIERX_APP_SCHEMA_INCLUDE
	${CMAKE_CURRENT_BINARY_DIR}/generated/NativeAppSchema.inc)
add_custom_command(
	OUTPUT ${FRONTIERX_APP_SCHEMA_INCLUDE}
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/generated
	COMMAND ${Python3_EXECUTABLE}
		${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability/embed_app_schema.py
		${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/data.i
		${FRONTIERX_APP_SCHEMA_INCLUDE}
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/data.i
		${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability/embed_app_schema.py
	VERBATIM)
add_library(frontierx_native_app_schema STATIC
	Code/App/Src/NativeAppSchema.cpp ${FRONTIERX_APP_SCHEMA_INCLUDE})
target_include_directories(frontierx_native_app_schema PUBLIC
	Code/App/Src/Include)
target_include_directories(frontierx_native_app_schema PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}/generated Code/Libs/Include)
target_compile_features(frontierx_native_app_schema PUBLIC cxx_std_17)
add_library(frontierx_native_global_data_format STATIC
	Code/App/Src/NativeGlobalDataFormat.cpp)
target_include_directories(frontierx_native_global_data_format PUBLIC
	Code/App/Src/Include)
target_include_directories(frontierx_native_global_data_format PRIVATE
	Code/App/DInclude Code/App/Src Code/Libs/Include)
target_compile_features(frontierx_native_global_data_format PUBLIC cxx_std_17)
add_library(frontierx_native_music_data_format STATIC
	Code/App/Src/NativeMusicDataFormat.cpp)
target_include_directories(frontierx_native_music_data_format PUBLIC
	Code/App/Src/Include)
target_include_directories(frontierx_native_music_data_format PRIVATE
	Code/App/DInclude Code/App/Src Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_native_music_data_format PUBLIC cxx_std_17)
target_link_libraries(frontierx_view_parser_probe PRIVATE
	frontierx_native_app_version
	frontierx_view_constructor frontierx_native_dacom_bootstrap frontierx_platform
	frontierx_native_global_data_format frontierx_native_music_data_format
	frontierx_native_app_schema
	frontierx_sol_t_schema_resource frontierx_sol_t_string_resource
	frontierx_script01_resources frontierx_script02_resources frontierx_script03_resources)
target_include_directories(frontierx_view_parser_probe PRIVATE
	Code/App/DInclude Code/App/Src Code/App/Src/Scripts/Sol_T)
target_compile_features(frontierx_view_parser_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_view_parser_probe)

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

add_library(frontierx_native_app_math STATIC
	Code/App/Src/NativeAppMath.cpp)
target_include_directories(frontierx_native_app_math PRIVATE Code/Libs/Include)
target_compile_features(frontierx_native_app_math PRIVATE cxx_std_17)

add_library(frontierx_native_app_globals STATIC
	Code/App/Src/NativeAppGlobals.cpp
	Code/App/Src/NativeAppHardpoints.cpp
	Code/App/Src/NativeAppHotRect.cpp)
target_link_libraries(frontierx_data_root_probe PRIVATE frontierx_native_app_globals)
target_include_directories(frontierx_native_app_globals PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude Code/Libs/Include)
target_compile_features(frontierx_native_app_globals PRIVATE cxx_std_17)

add_library(frontierx_native_app_diagnostics STATIC
	Code/App/Src/NativeAppDiagnostics.cpp)
target_include_directories(frontierx_native_app_diagnostics PRIVATE
	Code/App/Src Code/App/Src/Include Code/Libs/Include)
target_compile_features(frontierx_native_app_diagnostics PRIVATE cxx_std_17)

add_library(frontierx_native_app_render_state STATIC
	Code/App/Src/NativeAppRenderState.cpp)
target_include_directories(frontierx_native_app_render_state PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude Code/Libs/Include)
target_compile_features(frontierx_native_app_render_state PRIVATE cxx_std_17)
target_link_libraries(frontierx_native_app_render_state PUBLIC
	frontierx_native_app_diagnostics)

# The App's pane/rectangle callers need an executable native 2D path before
# the selected mission can enter its ObjectList rendering closure.
add_library(frontierx_native_app_2d_owner OBJECT
	Code/App/Src/NativeApp2DDraw.cpp)
target_include_directories(frontierx_native_app_2d_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude Code/Libs/Include)
target_compile_features(frontierx_native_app_2d_owner PRIVATE cxx_std_17)

# The production mission-object distance table is used by ObjectList's
# selection/movement logic and must retain its scalar evaluation order.
add_library(frontierx_app_grid_vector_owner OBJECT
	Code/App/Src/GridVector.cpp)
target_include_directories(frontierx_app_grid_vector_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude Code/Libs/Include)
target_compile_features(frontierx_app_grid_vector_owner PRIVATE cxx_std_17)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(frontierx_app_grid_vector_owner PRIVATE -ffp-contract=off)
endif()

# The original static circle table is consumed by BaseObject selection rings.
add_library(frontierx_app_circle_lines_owner OBJECT
	Code/App/Src/StacticInit.cpp)
target_include_directories(frontierx_app_circle_lines_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude Code/Libs/Include)
target_compile_features(frontierx_app_circle_lines_owner PRIVATE cxx_std_17)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(frontierx_app_circle_lines_owner PRIVATE -ffp-contract=off)
endif()

add_executable(frontierx_math_core_probe
	Tools/portability/math_core_probe.cpp
	$<TARGET_OBJECTS:frontierx_app_grid_vector_owner>
	$<TARGET_OBJECTS:frontierx_app_random_gen_owner>
	$<TARGET_OBJECTS:frontierx_app_circle_lines_owner>)
target_include_directories(frontierx_math_core_probe PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude Code/Libs/Include)

target_link_libraries(frontierx_math_core_probe PRIVATE
	frontierx_math frontierx_native_app_math)
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
add_library(frontierx_native_render_pipeline STATIC
	Code/Libs/Include/FrontierX/NativeRenderPipeline.h
	Code/Libs/Include/FrontierX/RenderPipelineAdapter.h
	Code/Libs/Src/Platform/NativeRenderPipeline.cpp)
target_include_directories(frontierx_native_render_pipeline PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/Code/Libs/Include)
target_link_libraries(frontierx_native_render_pipeline
	PUBLIC frontierx_dacom_core frontierx_render_submission)
target_compile_features(frontierx_native_render_pipeline PUBLIC cxx_std_17)
frontierx_enable_strict_warnings(frontierx_native_render_pipeline)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(frontierx_native_render_pipeline PRIVATE
		-Wno-delete-non-virtual-dtor)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_compile_options(frontierx_native_render_pipeline PRIVATE
		-Wno-delete-abstract-non-virtual-dtor
		-Wno-delete-non-virtual-dtor)
endif()
target_link_libraries(frontierx_native_render_primitive
	PUBLIC frontierx_native_render_pipeline)
add_library(frontierx_app_primitive_builder_owner OBJECT
	Code/App/Src/NativeAppPrimitiveBuilder.cpp)
target_include_directories(frontierx_app_primitive_builder_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude Code/Libs/Include)
target_compile_features(frontierx_app_primitive_builder_owner PRIVATE cxx_std_17)
# Source context cursors guard ObjComm/UnbornMeshList's logical cursor gate;
# SysMap and MScroll retain their original mini/edge-scroll resource identities.
file(GLOB frontierx_cursor_sources CONFIGURE_DEPENDS Code/App/Src/Res/*.cur)
set(frontierx_build_cursor_include ${CMAKE_CURRENT_BINARY_DIR}/generated/NativeAppBuildCursor.inc)
add_custom_command(OUTPUT ${frontierx_build_cursor_include}
	COMMAND ${Python3_EXECUTABLE}
		${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability/embed_app_build_cursor.py
		${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/Globals.rc
		${frontierx_build_cursor_include}
	DEPENDS Tools/portability/embed_app_build_cursor.py Code/App/Src/Globals.rc
		${frontierx_cursor_sources}
	VERBATIM)
add_library(frontierx_app_cursor_owner OBJECT
	Code/Libs/Src/Platform/NativeAppCursor.cpp ${frontierx_build_cursor_include})
target_include_directories(frontierx_app_cursor_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude Code/Libs/Include
	${CMAKE_CURRENT_BINARY_DIR}/generated)
target_compile_features(frontierx_app_cursor_owner PRIVATE cxx_std_17)
target_link_libraries(frontierx_app_cursor_owner PRIVATE PkgConfig::SDL3)
add_test(NAME frontierx_app_cursor_source_check
	COMMAND ${Python3_EXECUTABLE}
		${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability/embed_app_build_cursor.py
		--self-check ${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/Globals.rc)

# The production nugget manager owns mission-visible resources, updates and
# save/load; retain the original object and factory rather than a global stub.
add_library(frontierx_app_nugget_owner OBJECT
	Code/App/Src/Nugget.cpp)
target_include_directories(frontierx_app_nugget_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude Code/Libs/Include)
target_compile_features(frontierx_app_nugget_owner PRIVATE cxx_std_17)

# Production construction-preview and queued-mesh owner used by ObjectList.
add_library(frontierx_app_unborn_mesh_owner OBJECT
	Code/App/Src/UnbornMeshList.cpp)
target_include_directories(frontierx_app_unborn_mesh_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_unborn_mesh_owner PRIVATE cxx_std_17)

# Original mission command-history owner; native excludes its Win32 dialog.
add_library(frontierx_app_comm_track_owner OBJECT
	Code/App/Src/CommTrack.cpp)
target_include_directories(frontierx_app_comm_track_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_comm_track_owner PRIVATE cxx_std_17)

add_library(frontierx_app_fog_owner OBJECT
	Code/App/Src/FogOfWar.cpp)
target_include_directories(frontierx_app_fog_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude Code/Libs/Include)
target_compile_features(frontierx_app_fog_owner PRIVATE cxx_std_17)

# Sector::Load/New and normal movement require the original terrain owner.
# Keep its object target separate so its closure remains measurable.
add_library(frontierx_app_terrain_map_owner OBJECT
	Code/App/Src/TerrainMap.cpp)
target_include_directories(frontierx_app_terrain_map_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_terrain_map_owner PRIVATE cxx_std_17)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(frontierx_app_terrain_map_owner PRIVATE -ffp-contract=off)
endif()

# The original sector owner registers with App global startup and owns the
# mission's systems, terrain references and sector save/load state.
add_library(frontierx_app_sector_owner OBJECT
	Code/App/Src/Sector.cpp)
target_include_directories(frontierx_app_sector_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_sector_owner PRIVATE cxx_std_17)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(frontierx_app_sector_owner PRIVATE -ffp-contract=off)
endif()

# The production system-background owner is required by Sector::Load even
# when optional background effects are explicitly disabled for the slice.
add_library(frontierx_app_space_environment_owner OBJECT
	Code/App/Src/SpaceEnv.cpp)
target_include_directories(frontierx_app_space_environment_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_space_environment_owner PRIVATE cxx_std_17)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(frontierx_app_space_environment_owner PRIVATE -ffp-contract=off)
endif()

# The original App list owns mesh vertex-buffer restoration registrations.
add_library(frontierx_app_vertex_buffer_owner OBJECT
	Code/App/Src/VertexBuffer.cpp)
target_include_directories(frontierx_app_vertex_buffer_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_vertex_buffer_owner PRIVATE cxx_std_17)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(frontierx_app_vertex_buffer_owner PRIVATE -ffp-contract=off)
endif()

add_executable(frontierx_render_submission_probe
	Tools/portability/render_submission_probe.cpp
	$<TARGET_OBJECTS:frontierx_app_primitive_builder_owner>)
target_include_directories(frontierx_render_submission_probe PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude Code/Libs/Include)
target_link_libraries(frontierx_render_submission_probe
	PRIVATE frontierx_native_render_pipeline frontierx_native_render_primitive)
frontierx_enable_strict_warnings(frontierx_render_submission_probe)

add_executable(frontierx_render_pipeline_probe
	Tools/portability/render_pipeline_probe.cpp)
target_link_libraries(frontierx_render_pipeline_probe
	PRIVATE frontierx_native_render_pipeline)
frontierx_enable_strict_warnings(frontierx_render_pipeline_probe)

add_library(frontierx_render_smoke_scene STATIC
	Code/Libs/Include/FrontierX/RenderSmokeScene.h
	Code/Libs/Src/Platform/RenderSmokeScene.cpp)
target_include_directories(frontierx_render_smoke_scene PUBLIC Code/Libs/Include)
target_compile_features(frontierx_render_smoke_scene PUBLIC cxx_std_17)
frontierx_enable_strict_warnings(frontierx_render_smoke_scene)
target_link_libraries(frontierx_render_pipeline_probe
	PRIVATE frontierx_render_smoke_scene)

# Guard the ZBatcher declaration boundary and execute the production
# vertex-descriptor copier used by the neighboring vertex-buffer manager.
add_executable(frontierx_render_batcher_header_probe
	Code/Libs/Src/VertexBufferManager/CopyVertexBufferDesc.cpp
	Tools/portability/render_batcher_header_probe.cpp
	Tools/portability/render_batcher_myvertex_header.cpp)
target_include_directories(frontierx_render_batcher_header_probe
	PRIVATE Code/App/Src Code/App/Src/Include Code/App/DInclude Code/Libs/Include)
target_compile_features(frontierx_render_batcher_header_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_render_batcher_header_probe)
# Legacy math/graphics headers containing the batcher header chain carry
# intentional static arrays and copy-style idiom; the following warnings are
# sourced from those headers, not from the batcher interface boundary.
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(FRONTIERX_RB_HEADER_WARNS
        -Wno-unused-variable
        -Wno-deprecated-copy
        -Wno-pedantic
        -Wno-extra
        -Wno-unknown-pragmas)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        list(APPEND FRONTIERX_RB_HEADER_WARNS -Wno-template-id-cdtor)
    endif()
target_compile_options(frontierx_render_batcher_header_probe
		PRIVATE ${FRONTIERX_RB_HEADER_WARNS})
endif()

# Compile the supported App-side ZBatcher owner without claiming it links or runs yet.
add_library(frontierx_render_batcher_object OBJECT Code/App/Src/RenderBatcher.cpp)
target_include_directories(frontierx_render_batcher_object PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude Code/Libs/Include)
target_compile_features(frontierx_render_batcher_object PRIVATE cxx_std_17)

# The production MathLib accessor is an unresolved dependency of RenderBatcher.
add_library(frontierx_native_math_accessor STATIC Code/Libs/Src/x86Math/MathLib.cpp)
target_include_directories(frontierx_native_math_accessor PUBLIC Code/Libs/Include)
target_link_libraries(frontierx_native_math_accessor PUBLIC frontierx_native_dacom_bootstrap)
target_compile_features(frontierx_native_math_accessor PUBLIC cxx_std_17)

# Compile the production scalar I3DMathEngine provider used by engine transforms.
add_library(frontierx_x86_math_owner OBJECT Code/Libs/Src/x86Math/x86math.cpp)
target_include_directories(frontierx_x86_math_owner PRIVATE
	Code/Libs/Include Code/Libs/Src/x86Math)
target_compile_features(frontierx_x86_math_owner PRIVATE cxx_std_17)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(frontierx_x86_math_owner PRIVATE -ffp-contract=off)
endif()

add_executable(frontierx_math_accessor_probe Tools/portability/math_accessor_probe.cpp)
target_link_libraries(frontierx_math_accessor_probe PRIVATE frontierx_native_math_accessor)
target_compile_features(frontierx_math_accessor_probe PRIVATE cxx_std_17)

add_library(frontierx_bmpooljr_object OBJECT Code/App/Src/BMPoolJr.cpp)
target_include_directories(frontierx_bmpooljr_object PRIVATE
	Code/App/Src Code/App/Src/Include Code/Libs/Include)
target_compile_features(frontierx_bmpooljr_object PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_bmpooljr_object)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(frontierx_bmpooljr_object PRIVATE ${FRONTIERX_RB_HEADER_WARNS})
endif()

add_executable(frontierx_render_batcher_runtime_probe
	$<TARGET_OBJECTS:frontierx_render_batcher_object>
	$<TARGET_OBJECTS:frontierx_bmpooljr_object>
	$<TARGET_OBJECTS:frontierx_x86_math_owner>
	Tools/portability/render_batcher_runtime_probe.cpp)
target_include_directories(frontierx_render_batcher_runtime_probe PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude Code/Libs/Include)
target_link_libraries(frontierx_render_batcher_runtime_probe PRIVATE
	frontierx_native_math_accessor frontierx_native_render_primitive
	frontierx_render_smoke_scene frontierx_math)
target_compile_features(frontierx_render_batcher_runtime_probe PRIVATE cxx_std_17)

# Exercise the production manager through DACOM and the real native pipeline.
add_library(frontierx_vertex_buffer_manager_owner OBJECT
	Code/Libs/Src/VertexBufferManager/VertexBufferManager.cpp
	Code/Libs/Src/VertexBufferManager/CopyVertexBufferDesc.cpp)
target_include_directories(frontierx_vertex_buffer_manager_owner PRIVATE Code/Libs/Include)
target_compile_features(frontierx_vertex_buffer_manager_owner PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_vertex_buffer_manager_owner)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(frontierx_vertex_buffer_manager_owner PRIVATE
		${FRONTIERX_RB_HEADER_WARNS} -Wno-unused-parameter)
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(frontierx_vertex_buffer_manager_owner PRIVATE
		-Wno-delete-non-virtual-dtor)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_compile_options(frontierx_vertex_buffer_manager_owner PRIVATE
		-Wno-delete-abstract-non-virtual-dtor
		-Wno-delete-non-abstract-non-virtual-dtor)
endif()
add_executable(frontierx_vertex_buffer_manager_probe
	$<TARGET_OBJECTS:frontierx_vertex_buffer_manager_owner>
	Tools/portability/vertex_buffer_manager_probe.cpp)
target_link_libraries(frontierx_vertex_buffer_manager_probe PRIVATE
	frontierx_native_dacom_bootstrap frontierx_native_render_pipeline)
target_compile_features(frontierx_vertex_buffer_manager_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_vertex_buffer_manager_probe)

# The production StringTable is consumed by MScript through ISaverLoader.
# Link its actual aggregate against the native DACOM and UTF archive owners.
add_library(frontierx_string_table_owner OBJECT
	Code/Libs/Src/StringTable/StringTable.cpp)
target_include_directories(frontierx_string_table_owner PRIVATE Code/Libs/Include)
target_compile_features(frontierx_string_table_owner PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_string_table_owner)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(frontierx_string_table_owner PRIVATE
		-Wno-delete-non-virtual-dtor)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_compile_options(frontierx_string_table_owner PRIVATE
		-Wno-delete-abstract-non-virtual-dtor)
endif()
add_executable(frontierx_string_table_owner_probe
	$<TARGET_OBJECTS:frontierx_string_table_owner>
	Tools/portability/string_table_owner_probe.cpp)
target_include_directories(frontierx_string_table_owner_probe PRIVATE
	Code/App/Src/Include Code/Libs/Include)
target_link_libraries(frontierx_string_table_owner_probe PRIVATE
	frontierx_native_dacom_bootstrap frontierx_tinyxml frontierx_platform)
target_compile_features(frontierx_string_table_owner_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_string_table_owner_probe)

# Compile the production texture-library owner and link its aggregate into the
# material check, including the bounded UTF RGB565 upload path.
add_library(frontierx_texture_library_owner OBJECT
	Code/Libs/Src/Shading/TextureLibrary/TextureLibrary.cpp)
target_include_directories(frontierx_texture_library_owner PRIVATE Code/Libs/Include)
target_compile_features(frontierx_texture_library_owner PRIVATE cxx_std_17)

# Compile the active PolyMesh mesh owner used by the supported game. Its
# runtime material and renderer closure is not linked natively yet.
add_library(frontierx_polymesh_mesh_owner OBJECT
	Code/Libs/Src/RendComp/PolyMesh/Mesh.cpp
	Code/Libs/Src/RendComp/PolyMesh/sortq.cpp)
target_include_directories(frontierx_polymesh_mesh_owner PRIVATE
	Code/Libs/Include Code/Libs/Src/RendComp/PolyMesh)
target_compile_features(frontierx_polymesh_mesh_owner PRIVATE cxx_std_17)

# Compile the production renderer component used by RenderMgr's game mesh path.
add_library(frontierx_polymesh_component_owner OBJECT
	Code/Libs/Src/RendComp/PolyMesh/polymesh.cpp)
target_include_directories(frontierx_polymesh_component_owner PRIVATE
	Code/Libs/Include Code/Libs/Src/Include Code/Libs/Src/RendComp/PolyMesh)
target_compile_features(frontierx_polymesh_component_owner PRIVATE cxx_std_17)

# Compile the production lighting owner used by PolyMesh and App MeshRender.
add_library(frontierx_light_manager_owner OBJECT
	Code/Libs/Src/LightManager/LightManager.cpp)
target_include_directories(frontierx_light_manager_owner PRIVATE
	Code/Libs/Include Code/Libs/Src/Include Code/Libs/Src/LightManager)
target_compile_features(frontierx_light_manager_owner PRIVATE cxx_std_17)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(frontierx_light_manager_owner PRIVATE
		-Wall -Wextra -Wpedantic -ffp-contract=off)
endif()

add_executable(frontierx_light_manager_probe
	Tools/portability/light_manager_probe.cpp
	$<TARGET_OBJECTS:frontierx_light_manager_owner>
	$<TARGET_OBJECTS:frontierx_x86_math_owner>)
target_include_directories(frontierx_light_manager_probe PRIVATE
	Code/Libs/Include Code/Libs/Src/Include)
target_link_libraries(frontierx_light_manager_probe PRIVATE
	frontierx_native_dacom_bootstrap frontierx_native_math_accessor frontierx_math)
target_compile_features(frontierx_light_manager_probe PRIVATE cxx_std_17)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(frontierx_light_manager_probe PRIVATE
		-Wall -Wextra -Wpedantic)
endif()

# Compile the active App mesh caller beside its production material/renderer
# owners. Effect passes remain explicitly unsupported on the native path.
add_library(frontierx_app_mesh_render_owner OBJECT
	Code/App/Src/MeshRender.cpp
	Code/App/Src/MeshObj.cpp
	Code/App/Src/MeshInfo.cpp
	Code/App/Src/SuperTrans.cpp)
target_include_directories(frontierx_app_mesh_render_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include
	Code/Libs/Src/RendComp/PolyMesh Code/Libs/Src/TextureManager)
target_compile_features(frontierx_app_mesh_render_owner PRIVATE cxx_std_17)

# Production ParticleEffect aggregate and its filter/parameter implementation.
add_library(frontierx_particle_owner OBJECT
	Code/Libs/Src/ParticleEffect/ParticleEffect.cpp
	Code/Libs/Src/ParticleEffect/Particles.cpp
	Code/Libs/Src/ParticleEffect/ParticleParamFunctions.cpp
	Code/Launcher/NativeParticleValidation.cpp)
target_include_directories(frontierx_particle_owner PRIVATE
	Code/Libs/Include Code/Libs/Src/Include Code/Libs/Src/ParticleEffect)
target_compile_features(frontierx_particle_owner PRIVATE cxx_std_17)

# Production MeshManager aggregate, archetypes and instances.
add_library(frontierx_mesh_manager_owner OBJECT
	Code/Libs/Src/MeshManager/MeshManager.cpp
	Code/Libs/Src/MeshManager/MeshArch.cpp
	Code/Libs/Src/MeshManager/MeshInstance.cpp)
target_include_directories(frontierx_mesh_manager_owner PRIVATE
	Code/Libs/Include Code/Libs/Src/Include Code/Libs/Src/MeshManager)
target_compile_features(frontierx_mesh_manager_owner PRIVATE cxx_std_17)

# Production animation aggregate, script/channel runtime and motion stack.
add_library(frontierx_animation_owner OBJECT
	Code/Libs/Src/Anim/Anim.cpp
	Code/Libs/Src/Anim/AnimComponent.cpp
	Code/Libs/Src/Anim/channel.cpp
	Code/Libs/Src/Anim/mstack.cpp)
target_include_directories(frontierx_animation_owner PRIVATE
	Code/Libs/Include Code/Libs/Src/Include Code/Libs/Src/Anim)
target_compile_features(frontierx_animation_owner PRIVATE cxx_std_17)

# Production IChannel2 archive decoder used by the animation owner.
add_library(frontierx_channel_owner OBJECT
	Code/Libs/Src/Channel/channel.cpp)
target_include_directories(frontierx_channel_owner PRIVATE
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_channel_owner PRIVATE cxx_std_17)

add_library(frontierx_native_animation_validation OBJECT
	Code/Launcher/NativeAnimationValidation.cpp)
target_include_directories(frontierx_native_animation_validation PRIVATE
	Code/Launcher Code/Libs/Include)
target_compile_features(frontierx_native_animation_validation PRIVATE cxx_std_17)

# Production Engine hardpoint aggregate and archetype metadata.
add_library(frontierx_hardpoint_owner OBJECT
	Code/Libs/Src/EngComps/Hardpoint/HardPoint.cpp)
target_include_directories(frontierx_hardpoint_owner PRIVATE
	Code/Libs/Include Code/Libs/Src/Include Code/Libs/Src/EngComps/Hardpoint)
target_compile_features(frontierx_hardpoint_owner PRIVATE cxx_std_17)

# Production render component requested by the source profile.
add_library(frontierx_optics_owner OBJECT
	Code/Libs/Src/RendComp/Optics/optics.cpp
	Code/Libs/Src/RendComp/Optics/ParticleSystemArchetype.cpp
	Code/Libs/Src/RendComp/Optics/ParticleSystemInstance.cpp)
target_include_directories(frontierx_optics_owner PRIVATE
	Code/Libs/Include Code/Libs/Src/Include Code/Libs/Src/RendComp/Optics)
target_compile_features(frontierx_optics_owner PRIVATE cxx_std_17)

# Compile the active App startup container used by GameSystem::startup.
# Its CQPipeline wrapper and full system profile are separate integration gates.
add_library(frontierx_app_system_container_owner OBJECT
	Code/App/Src/SysContainer.cpp)
target_include_directories(frontierx_app_system_container_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_system_container_owner PRIVATE cxx_std_17)

# Compile the production general-data owner consumed by MGlobals.
add_library(frontierx_app_gen_data_owner OBJECT
	Code/App/Src/GenData.cpp)
target_include_directories(frontierx_app_gen_data_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_gen_data_owner PRIVATE cxx_std_17)

# Compile the production mission-global owner; linking its App services is
# the next integration gate before Mission::Reload can enter MGlobals::Load.
add_library(frontierx_app_mglobals_owner OBJECT
	Code/App/Src/MGlobals.cpp)
target_include_directories(frontierx_app_mglobals_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include
	Code/Libs/Src/Viewer/DOCUVIEW)
target_compile_features(frontierx_app_mglobals_owner PRIVATE cxx_std_17)

# Object-pointer watcher lifetime is needed by the camera manager without MGlobals.
add_library(frontierx_app_objwatch_owner OBJECT
	Code/App/Src/Objwatch.cpp)
target_include_directories(frontierx_app_objwatch_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include
	Code/Libs/Src/Viewer/DOCUVIEW)
target_compile_features(frontierx_app_objwatch_owner PRIVATE cxx_std_17)

# Compile the production object/archetype owner used by mission loading.
# Its App service link closure remains a separate integration gate.
add_library(frontierx_app_base_object_owner OBJECT
	Code/App/Src/BaseObject.cpp)
target_include_directories(frontierx_app_base_object_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include
	Code/Libs/Src/Viewer/DOCUVIEW)
target_compile_features(frontierx_app_base_object_owner PRIVATE cxx_std_17)

add_library(frontierx_app_object_list_owner OBJECT
	Code/App/Src/ObjList.cpp
	Code/App/Src/GroupObj.cpp
	Tools/portability/app_object_template_header.cpp)
target_include_directories(frontierx_app_object_list_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include
	Code/Libs/Src/Viewer/DOCUVIEW)
target_compile_features(frontierx_app_object_list_owner PRIVATE cxx_std_17)

# The original Forger factory registers itself before ObjectList loads the
# selected training mission. Its ship and movement closure is linked below.
add_library(frontierx_app_fabricator_owner OBJECT
	Code/App/Src/Fabricator.cpp)
target_include_directories(frontierx_app_fabricator_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_fabricator_owner PRIVATE cxx_std_17)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_compile_options(frontierx_app_fabricator_owner PRIVATE
		-fdelayed-template-parsing -fms-extensions)
endif()

add_library(frontierx_app_spaceship_owner OBJECT
	Code/App/Src/SpaceShip.cpp
	Code/App/Src/MineLayer.cpp
	Code/App/Src/SupplyShip.cpp
	Code/App/Src/ObjMove.cpp
	Code/App/Src/ObjControl.cpp
	Code/App/Src/Damage.cpp
	Code/App/Src/CQExtent.cpp
	Code/App/Src/SimpleMesh.cpp
	Code/App/Src/Blinkers.cpp
	Code/App/Src/Trail.cpp
	Code/App/Src/explosion.cpp
	Code/App/Src/debris.cpp
	Code/App/Src/MeshExplode.cpp)
target_include_directories(frontierx_app_spaceship_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_spaceship_owner PRIVATE cxx_std_17)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_compile_options(frontierx_app_spaceship_owner PRIVATE
		-fdelayed-template-parsing -fms-extensions)
endif()

add_library(frontierx_app_banker_owner OBJECT
	Code/App/Src/Banker.cpp)
target_include_directories(frontierx_app_banker_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_banker_owner PRIVATE cxx_std_17)

add_library(frontierx_app_jumpgate_owner OBJECT
	Code/App/Src/jumpgate.cpp)
target_include_directories(frontierx_app_jumpgate_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_jumpgate_owner PRIVATE cxx_std_17)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_compile_options(frontierx_app_jumpgate_owner PRIVATE
		-fdelayed-template-parsing -fms-extensions)
endif()

add_library(frontierx_app_waypoint_owner OBJECT
	Code/App/Src/Waypoint.cpp)
target_include_directories(frontierx_app_waypoint_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_waypoint_owner PRIVATE cxx_std_17)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_compile_options(frontierx_app_waypoint_owner PRIVATE
		-fdelayed-template-parsing -fms-extensions)
endif()

add_library(frontierx_app_trigger_owner OBJECT
	Code/App/Src/Trigger.cpp)
target_include_directories(frontierx_app_trigger_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_trigger_owner PRIVATE cxx_std_17)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_compile_options(frontierx_app_trigger_owner PRIVATE
		-fdelayed-template-parsing -fms-extensions)
endif()

add_library(frontierx_app_gunboat_owner OBJECT
	Code/App/Src/Gunboat.cpp
	Code/App/Src/FlagShip.cpp
	Code/App/Src/SellPlat.cpp
	Code/App/Src/Aegis.cpp
	Code/App/Src/AreaEffectBolt.cpp
	Code/App/Src/DumbReconProbe.cpp
	Code/App/Src/Troopship.cpp
	Code/App/Src/ReconProbe.cpp
	Code/App/Src/TroopPod.cpp
	Code/App/Src/WormholeLauncher.cpp
	Code/App/Src/WormholeBlast.cpp
	Code/App/Src/MultiCloakLauncher.cpp
	Code/App/Src/Synthesis.cpp
	Code/App/Src/FighterWing.cpp
	Code/App/Src/Fighter.cpp
	Code/App/Src/EngineTrail.cpp
	Code/App/Src/Projectile.cpp
	Code/App/Src/VLaunch.cpp
	Code/App/Src/CloakLauncher.cpp
	Code/App/Src/AutoCannon.cpp
	Code/App/Src/AirDefense.cpp
	Code/App/Src/ReconLaunch.cpp
	Code/App/Src/StatisBolt.cpp
	Code/App/Src/RepellentCloud.cpp
	Code/App/Src/RepulsorWave.cpp
	Code/App/Src/Destabilizer.cpp
	Code/App/Src/Tractor.cpp
	Code/App/Src/Mimic.cpp
	Code/App/Src/PingLauncher.cpp
	Code/App/Src/Zealot.cpp
	Code/App/Src/TalorianLauncher.cpp
	Code/App/Src/TalorianEffect.cpp
	Code/App/Src/HarvestShip.cpp
	Code/App/Src/Turret.cpp
	Code/App/Src/FancyLaunch.cpp
	Code/App/Src/arc.cpp
	Code/App/Src/beam.cpp
	Code/App/Src/MassDisruptor.cpp
	Code/App/Src/GattlingBeam.cpp
	Code/App/Src/PlasmaBolt.cpp)
target_include_directories(frontierx_app_gunboat_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_gunboat_owner PRIVATE cxx_std_17)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_compile_options(frontierx_app_gunboat_owner PRIVATE
		-fdelayed-template-parsing -fms-extensions)
endif()

add_library(frontierx_app_build_support_platform_owner OBJECT
	Code/App/Src/BuildSupPlat.cpp
	Code/App/Src/JumpPlat.cpp
	Code/App/Src/GunPlat.cpp
	Code/App/Src/BuildPlat.cpp
	Code/App/Src/RefinePlat.cpp
	Code/App/Src/RepairPlat.cpp
	Code/App/Src/BuildShip.cpp
	Code/App/Src/SolarianBuild.cpp
	Code/App/Src/TerranBuild.cpp
	Code/App/Src/TerranDrone.cpp
	Code/App/Src/ScriptObject.cpp
	Code/App/Src/MantisBuild.cpp
	Code/App/Src/SpiderDrone.cpp)
target_include_directories(frontierx_app_build_support_platform_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_build_support_platform_owner PRIVATE cxx_std_17)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_compile_options(frontierx_app_build_support_platform_owner PRIVATE
		-fdelayed-template-parsing -fms-extensions)
endif()

# The training mission's build-support platforms require real planet instances.
add_library(frontierx_app_planetoid_owner OBJECT
	Code/App/Src/Planetoid.cpp)
target_include_directories(frontierx_app_planetoid_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_planetoid_owner PRIVATE cxx_std_17)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_compile_options(frontierx_app_planetoid_owner PRIVATE
		-fdelayed-template-parsing -fms-extensions)
endif()

# Compile the production mission owner before closing its App service link graph.
add_library(frontierx_app_object_map_owner OBJECT
	Code/App/Src/ObjMap.cpp
	Code/App/Src/ObjMapIterator.cpp)
target_include_directories(frontierx_app_object_map_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include
	Code/Libs/Src/Viewer/DOCUVIEW)
target_compile_features(frontierx_app_object_map_owner PRIVATE cxx_std_17)

add_library(frontierx_app_event_scheduler_owner OBJECT
	Code/App/Src/EventScheduler.cpp)
target_include_directories(frontierx_app_event_scheduler_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include
	Code/Libs/Src/Viewer/DOCUVIEW)
target_compile_features(frontierx_app_event_scheduler_owner PRIVATE cxx_std_17)

add_library(frontierx_app_movie_camera_manager_owner OBJECT
	Code/App/Src/MovieCamera.cpp)
target_include_directories(frontierx_app_movie_camera_manager_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include
	Code/Libs/Src/Viewer/DOCUVIEW)
target_compile_features(frontierx_app_movie_camera_manager_owner PRIVATE cxx_std_17)


add_library(frontierx_app_mission_owner OBJECT
	Code/App/Src/GameProgress.cpp
	Code/App/Src/Mission.cpp)
target_include_directories(frontierx_app_mission_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include
	Code/Libs/Src/Viewer/DOCUVIEW)
target_compile_features(frontierx_app_mission_owner PRIVATE cxx_std_17)

add_library(frontierx_app_effect_player_owner OBJECT
	Code/App/Src/EffectPlayer.cpp)
target_include_directories(frontierx_app_effect_player_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_effect_player_owner PRIVATE cxx_std_17)

add_library(frontierx_app_field_manager_owner OBJECT
	Code/App/Src/cloud.cpp)
target_include_directories(frontierx_app_field_manager_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_field_manager_owner PRIVATE cxx_std_17)

add_library(frontierx_app_minefield_owner OBJECT
	Code/App/Src/MineField.cpp)
target_include_directories(frontierx_app_minefield_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_minefield_owner PRIVATE cxx_std_17)

add_library(frontierx_app_antimatter_owner OBJECT
	Code/App/Src/AntiMatter.cpp)
target_include_directories(frontierx_app_antimatter_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_antimatter_owner PRIVATE cxx_std_17)

add_library(frontierx_app_blast_owner OBJECT
	Code/App/Src/blast.cpp
	Code/App/Src/FireBall.cpp
	Code/App/Src/ParticleObj.cpp)
target_include_directories(frontierx_app_blast_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_blast_owner PRIVATE cxx_std_17)

add_library(frontierx_app_light_owner OBJECT
	Code/App/Src/light.cpp)
target_include_directories(frontierx_app_light_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_light_owner PRIVATE cxx_std_17)

add_library(frontierx_app_map_gen_owner OBJECT
	Code/App/Src/MapGen.cpp)
target_include_directories(frontierx_app_map_gen_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_map_gen_owner PRIVATE cxx_std_17)

add_library(frontierx_app_morph_mesh_owner OBJECT
	Code/App/Src/MorphMesh.cpp)
target_include_directories(frontierx_app_morph_mesh_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_morph_mesh_owner PRIVATE cxx_std_17)

add_library(frontierx_native_app_profile_parser OBJECT
	Code/App/Src/NativeAppProfileParser.cpp)
target_include_directories(frontierx_native_app_profile_parser PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_native_app_profile_parser PRIVATE cxx_std_17)

add_library(frontierx_native_fleet_group_pool OBJECT
	Code/App/Src/NativeFleetGroupPool.cpp Code/App/Src/FleetGroupPool.inc)
target_include_directories(frontierx_native_fleet_group_pool PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_native_fleet_group_pool PRIVATE cxx_std_17)

add_library(frontierx_app_random_gen_owner OBJECT
	Code/App/Src/RandomGen.cpp)
target_include_directories(frontierx_app_random_gen_owner PRIVATE
	Code/App/Src Code/Libs/Include)
target_compile_features(frontierx_app_random_gen_owner PRIVATE cxx_std_17)

add_library(frontierx_app_anim2d_owner OBJECT
	Code/App/Src/anim2d.cpp)
target_include_directories(frontierx_app_anim2d_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_anim2d_owner PRIVATE cxx_std_17)

add_library(frontierx_app_dust_owner OBJECT
	Code/App/Src/Dust.cpp)
target_include_directories(frontierx_app_dust_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_dust_owner PRIVATE cxx_std_17)

# Script01 requests the original Mantis frigate-rush strategy. Retain the
# production dispatcher and its strategy factories, including save restoration.
add_library(frontierx_native_player_ai_boundary OBJECT
	Code/App/Src/SPlayerAI.cpp
	Code/App/Src/MFrigateRush.cpp
	Code/App/Src/MStandardMantisAI.cpp
	Code/App/Src/MStandardSolarianAI.cpp
	Code/App/Src/MForwardBuild.cpp
	Code/App/Src/MDreadnoughts.cpp
	Code/App/Src/MCorvetteRush.cpp
	Code/App/Src/MFortress.cpp
	Code/App/Src/MSwarm.cpp
	Code/App/Src/MForgers.cpp
	Code/App/Src/MDeny.cpp)
target_include_directories(frontierx_native_player_ai_boundary PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_native_player_ai_boundary PRIVATE cxx_std_17)

add_library(frontierx_app_music_manager_owner OBJECT
	Code/App/Src/MusicManager.cpp)
target_include_directories(frontierx_app_music_manager_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include
	Code/Libs/Src/Viewer/DOCUVIEW)
target_compile_features(frontierx_app_music_manager_owner PRIVATE cxx_std_17)

add_library(frontierx_app_modal_owner OBJECT Code/App/Src/Modal.cpp)
target_include_directories(frontierx_app_modal_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_modal_owner PRIVATE cxx_std_17)

# The original Windows resource remains generated as sfxdata.dat. Native
# builds compile the same source ID-to-file table without embedding retail data.
set(frontierx_native_sfx_table ${CMAKE_CURRENT_BINARY_DIR}/NativeSfxTable.inc)
add_custom_command(OUTPUT ${frontierx_native_sfx_table}
	COMMAND ${Python3_EXECUTABLE}
		${CMAKE_CURRENT_SOURCE_DIR}/Tools/xmiff2/xmiff2.py
		${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/sfxdata.xmf
		${CMAKE_CURRENT_SOURCE_DIR}/Code/App/DInclude/Sfxid.h
		${frontierx_native_sfx_table}
	DEPENDS Tools/xmiff2/xmiff2.py Code/App/Src/sfxdata.xmf
		Code/App/DInclude/Sfxid.h
	VERBATIM)
add_library(frontierx_native_sfx_owner OBJECT
	Code/App/Src/NativeAppSfx.cpp ${frontierx_native_sfx_table})
target_include_directories(frontierx_native_sfx_owner PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_native_sfx_owner PRIVATE cxx_std_17)

# Native App media owners implement the existing mission-facing interfaces;
# speech goes through the required IStreamer, while unsupported display paths fail.
add_library(frontierx_native_mission_media_owner OBJECT
	Code/App/Src/NativeAppMissionSound.cpp
	Code/App/Src/LFParser.cpp
	Code/App/Src/NativeAppMissionTeletype.cpp
	Code/App/Src/StatusBar.cpp
	Code/App/Src/Hintbox.cpp
	Code/App/Src/ScrollingText.cpp
	Code/App/Src/Subtitle.cpp
	Code/App/Src/Teletype.cpp
	Code/App/Src/NativeAppFont.cpp
	Code/App/Src/UnitComm.cpp)
target_include_directories(frontierx_native_mission_media_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_native_mission_media_owner PRIVATE cxx_std_17)
set(FRONTIERX_TEST_FONT_FILE "" CACHE FILEPATH "External font for the optional native shell lifecycle check")
option(FRONTIERX_BUILD_NATIVE_FONTS "Enable the optional FreeType App font adapter" OFF)
if(FRONTIERX_TEST_FONT_FILE AND NOT FRONTIERX_BUILD_NATIVE_FONTS)
    message(FATAL_ERROR "FRONTIERX_TEST_FONT_FILE requires FRONTIERX_BUILD_NATIVE_FONTS")
endif()
if(FRONTIERX_BUILD_NATIVE_FONTS)
    find_package(Freetype 2.14 REQUIRED)
    target_link_libraries(frontierx_native_mission_media_owner PRIVATE Freetype::Freetype)
    target_compile_definitions(frontierx_native_mission_media_owner PRIVATE FRONTIERX_NATIVE_FREETYPE=1)
endif()

# Production StringData owns the fixed-width unit speech slots in StringPack.db.
add_library(frontierx_app_string_data_owner OBJECT Code/App/Src/StringData.cpp)
target_include_directories(frontierx_app_string_data_owner PRIVATE
    Code/App/Src Code/App/Src/Include Code/App/DInclude
    Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_string_data_owner PRIVATE cxx_std_17)
target_sources(frontierx_data_root_probe PRIVATE
    $<TARGET_OBJECTS:frontierx_app_string_data_owner>)

# Bind the selected mission's native ISystemMap contract without Win32 GDI/UI.
add_library(frontierx_native_system_map_owner OBJECT
	Code/Libs/Src/Platform/NativeSystemMap.cpp)
target_include_directories(frontierx_native_system_map_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude Code/Libs/Include)
target_compile_features(frontierx_native_system_map_owner PRIVATE cxx_std_17)
target_sources(frontierx_data_root_probe PRIVATE
	$<TARGET_OBJECTS:frontierx_native_system_map_owner>
	$<TARGET_OBJECTS:frontierx_app_global_startup_owner>)
target_include_directories(frontierx_data_root_probe PRIVATE
	Code/App/Src Code/App/DInclude)
# The bounded source camera-scroll branch reads the production defaults owner.
target_link_libraries(frontierx_data_root_probe PRIVATE frontierx_native_user_defaults)

# Compile the production operation/simulation owner used by Mission::Update.
add_library(frontierx_app_object_comm_owner OBJECT EXCLUDE_FROM_ALL
	Code/App/Src/ObjComm.cpp)
target_include_directories(frontierx_app_object_comm_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include
	Code/Libs/Src/Viewer/DOCUVIEW)
target_compile_features(frontierx_app_object_comm_owner PRIVATE cxx_std_17)

add_library(frontierx_app_ui_anim_owner OBJECT EXCLUDE_FROM_ALL
	Code/App/Src/UIAnim.cpp)
target_include_directories(frontierx_app_ui_anim_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include
	Code/Libs/Src/Viewer/DOCUVIEW)
target_compile_features(frontierx_app_ui_anim_owner PRIVATE cxx_std_17)

add_library(frontierx_app_anim_obj_owner OBJECT EXCLUDE_FROM_ALL
	Code/App/Src/AnimObj.cpp)
target_include_directories(frontierx_app_anim_obj_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include
	Code/Libs/Src/Viewer/DOCUVIEW)
target_compile_features(frontierx_app_anim_obj_owner PRIVATE cxx_std_17)

add_library(frontierx_app_op_agent_owner OBJECT
	Code/App/Src/OpAgent.cpp)
target_include_directories(frontierx_app_op_agent_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include
	Code/Libs/Src/Viewer/DOCUVIEW)
target_compile_features(frontierx_app_op_agent_owner PRIVATE cxx_std_17)

# Compile the production single-player packet queue used by NetPacket/OpAgent.
add_library(frontierx_app_net_buffer_owner OBJECT
	Code/App/Src/NetBuffer.cpp)
target_include_directories(frontierx_app_net_buffer_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include
	Code/Libs/Src/Viewer/DOCUVIEW)
target_compile_features(frontierx_app_net_buffer_owner PRIVATE cxx_std_17)

# Compile the production packet manager that routes offline NETF_ECHO input.
add_library(frontierx_app_net_packet_owner OBJECT
	Code/App/Src/NetPacket.cpp)
target_include_directories(frontierx_app_net_packet_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include
	Code/Libs/Src/Viewer/DOCUVIEW)
target_compile_features(frontierx_app_net_packet_owner PRIVATE cxx_std_17)

# Retain the existing isolated packet/manager-save closure. The full mission
# owner above registers MovieCameraFactory against the production ObjectList;
# this check has no ObjectList and does not claim to exercise that factory.
add_library(frontierx_app_movie_camera_manager_check_owner OBJECT Code/App/Src/MovieCamera.cpp)
target_include_directories(frontierx_app_movie_camera_manager_check_owner PRIVATE
    $<TARGET_PROPERTY:frontierx_app_movie_camera_manager_owner,INCLUDE_DIRECTORIES>)
target_compile_features(frontierx_app_movie_camera_manager_check_owner PRIVATE cxx_std_17)
target_compile_definitions(frontierx_app_movie_camera_manager_check_owner PRIVATE
    FRONTIERX_CAMERA_MANAGER_CHECK=1)

add_executable(frontierx_net_buffer_probe
	Tools/portability/net_buffer_probe.cpp
	Tools/portability/camera_save_support.cpp
	# MovieCamera's UBSan vptr checks require the original IBaseObject RTTI owner.
	$<TARGET_OBJECTS:frontierx_app_base_object_owner>
	$<TARGET_OBJECTS:frontierx_app_primitive_builder_owner>
	$<TARGET_OBJECTS:frontierx_app_circle_lines_owner>
	$<TARGET_OBJECTS:frontierx_app_net_buffer_owner>
	$<TARGET_OBJECTS:frontierx_app_net_packet_owner>
	$<TARGET_OBJECTS:frontierx_app_movie_camera_manager_check_owner>
	$<TARGET_OBJECTS:frontierx_app_objwatch_owner>
	$<TARGET_OBJECTS:frontierx_app_mesh_render_owner>
	$<TARGET_OBJECTS:frontierx_app_global_startup_owner>)
target_include_directories(frontierx_net_buffer_probe PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_link_libraries(frontierx_net_buffer_probe PRIVATE
	frontierx_native_search
	frontierx_native_app_globals frontierx_native_app_diagnostics
	frontierx_native_user_defaults frontierx_native_dacom_bootstrap
	frontierx_event_system frontierx_platform
	frontierx_native_math_accessor frontierx_math
	frontierx_native_app_math frontierx_native_app_render_state)
target_compile_features(frontierx_net_buffer_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_net_buffer_probe)

# Share the original App registrar and two-phase startup contract with Linux.
add_library(frontierx_app_global_startup_owner OBJECT
	Code/App/Src/GlobalStartup.cpp
	Code/App/Src/NativeUnitCatalog.cpp)
target_include_directories(frontierx_app_global_startup_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_global_startup_owner PRIVATE cxx_std_17)

# Original command-menu owner required by Sol_Train's Oxidator build step.
add_library(frontierx_app_toolbar_owner OBJECT
	Code/App/Src/CMenu_Fabricator.cpp
	Code/App/Src/CMenu_BuildInd.cpp
	Code/App/Src/CMenu_MulMix.cpp
	Code/App/Src/CMenu_FindButtons.cpp
	Code/App/Src/CMenu_Ind.cpp
	Code/App/Src/CMenu_ResBuild.cpp
	Code/App/Src/CMenu_GenPlat.cpp
	Code/App/Src/CMenu_Research.cpp
	Code/App/Src/CMenu_None.cpp
	Code/App/Src/Menu_Toolbar.cpp
	Code/App/Src/menu.cpp
	Code/App/Src/Menu_Pause.cpp
	Code/App/Src/Menu_Objectives.cpp
	Code/App/Src/Button2.cpp
	Code/App/Src/Animate.cpp
	Code/App/Src/Menu_Briefing.cpp
	Code/App/Src/Menu_EndGame.cpp
	Code/App/Src/Menu_Confirm.cpp
	Code/App/Src/Menu_campaign.cpp
	Code/App/Src/Menu_mission.cpp
	Code/App/Src/Listbox.cpp
	Code/App/Src/ScrollBar.cpp
	Code/App/Src/SysMap.cpp
	Code/App/Src/DrawAgent16.cpp
	Code/App/Src/ShapeLoader.cpp
	Code/App/Src/DrawAgent.cpp
	Code/App/Src/HotButton.cpp
	Code/App/Src/BuildButton.cpp
	Code/App/Src/MultiHotButton.cpp
	Code/App/Src/Static.cpp
	Code/App/Src/ProgressStatic.cpp
	Code/App/Src/Edit2.cpp
	Code/App/Src/HotStatic.cpp
	Code/App/Src/ShipSilButton.cpp
	Code/App/Src/TabButton.cpp
	Code/App/Src/TabControl.cpp
	Code/App/Src/InterfaceManager.cpp
	Code/App/Src/Icon.cpp
	Code/App/Src/QueueControl.cpp
	Code/App/Src/ResearchButton.cpp)
target_include_directories(frontierx_app_toolbar_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_toolbar_owner PRIVATE cxx_std_17)

# Original camera and mouse-edge scroll owners for the native mission.
add_library(frontierx_app_camera_owner OBJECT
	Code/App/Src/Camera.cpp
	Code/App/Src/MScroll.cpp)
target_include_directories(frontierx_app_camera_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include)
target_compile_features(frontierx_app_camera_owner PRIVATE cxx_std_17)

# Guard the production App registrar order across the 26-entry node boundary.
add_executable(frontierx_app_global_startup_probe
	Tools/portability/app_global_startup_probe.cpp
	$<TARGET_OBJECTS:frontierx_app_global_startup_owner>)
target_include_directories(frontierx_app_global_startup_probe PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude Code/Libs/Include)
target_compile_features(frontierx_app_global_startup_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_app_global_startup_probe)

# Cursor creation needs a real host display; SDL's dummy driver rejects it.
add_executable(frontierx_app_cursor_probe
	Tools/portability/native_app_cursor_probe.cpp
	$<TARGET_OBJECTS:frontierx_app_global_startup_owner>
	$<TARGET_OBJECTS:frontierx_app_cursor_owner>)
target_include_directories(frontierx_app_cursor_probe PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude Code/Libs/Include)
target_link_libraries(frontierx_app_cursor_probe PRIVATE
	frontierx_native_app_globals PkgConfig::SDL3)
target_compile_features(frontierx_app_cursor_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_app_cursor_probe)

# Compile the production engine that aggregates RenderMgr and dispatches
# archetype and instance ownership through the game component graph.
add_library(frontierx_engine_owner OBJECT
	Code/Libs/Src/Engine/engine.cpp)
target_include_directories(frontierx_engine_owner PRIVATE
	Code/Libs/Include Code/Libs/Src/Include Code/Libs/Src/Engine)
target_compile_features(frontierx_engine_owner PRIVATE cxx_std_17)

# Compile the production engine component that dispatches game render
# archetypes and instances to PolyMesh.
add_library(frontierx_render_manager_owner OBJECT
	Code/Libs/Src/EngComps/RenderMgr/RenderMgr.cpp
	Code/Libs/Src/EngComps/RenderMgr/chull.cpp)
target_include_directories(frontierx_render_manager_owner PRIVATE
	Code/Libs/Include Code/Libs/Src/Include Code/Libs/Src/EngComps/RenderMgr)
target_compile_features(frontierx_render_manager_owner PRIVATE cxx_std_17)

# Compile the game's material-manager owner without its Win32 dialog and DLL
# registration. Linking its material, texture and script runtime follows.
add_library(frontierx_game_material_manager_owner OBJECT
	Code/Libs/Src/MaterialManager/MaterialManager.cpp
	Code/Libs/Src/MaterialManager/Material.cpp
	Code/Libs/Src/MaterialManager/Modifier.cpp)
# The CQ2 manager's global Material is unrelated to the legacy mesh Material.
# Distinct native symbol names keep their destructors separate in one ELF.
target_compile_definitions(frontierx_game_material_manager_owner PRIVATE
	Material=NativeGameMaterial)
target_include_directories(frontierx_game_material_manager_owner PRIVATE
	Code/Libs/Include Code/Libs/Src/MaterialManager)
target_compile_features(frontierx_game_material_manager_owner PRIVATE cxx_std_17)

# Compile the game's production ITManager with the TGA/BMP readers used by
# this native material closure. VFX uses the bounded native SHP decoder.
add_library(frontierx_game_texture_manager_owner OBJECT
	Code/Libs/Src/TextureManager/TextureManager.cpp
	Code/Libs/Src/TextureManager/Tgaread.cpp
	Code/Libs/Src/TextureManager/BmpRead.cpp
	Code/App/Src/VfxRead.cpp)
target_include_directories(frontierx_game_texture_manager_owner PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/TextureManager)
target_compile_features(frontierx_game_texture_manager_owner PRIVATE cxx_std_17)

# A separate executable avoids combining the game's IMaterial with the
# unrelated shading-library IMaterial in one test program.
add_executable(frontierx_game_material_manager_probe
	$<TARGET_OBJECTS:frontierx_hardpoint_owner>
	$<TARGET_OBJECTS:frontierx_mesh_manager_owner>
	$<TARGET_OBJECTS:frontierx_animation_owner>
	$<TARGET_OBJECTS:frontierx_channel_owner>
	$<TARGET_OBJECTS:frontierx_app_anim2d_owner>
	$<TARGET_OBJECTS:frontierx_app_global_startup_owner>
	$<TARGET_OBJECTS:frontierx_native_app_2d_owner>
	$<TARGET_OBJECTS:frontierx_app_primitive_builder_owner>
	$<TARGET_OBJECTS:frontierx_game_material_manager_owner>
	$<TARGET_OBJECTS:frontierx_app_mesh_render_owner>
	$<TARGET_OBJECTS:frontierx_game_texture_manager_owner>
	$<TARGET_OBJECTS:frontierx_polymesh_mesh_owner>
	$<TARGET_OBJECTS:frontierx_polymesh_component_owner>
	$<TARGET_OBJECTS:frontierx_engine_owner>
	$<TARGET_OBJECTS:frontierx_render_manager_owner>
	$<TARGET_OBJECTS:frontierx_x86_math_owner>
	$<TARGET_OBJECTS:frontierx_light_manager_owner>
	$<TARGET_OBJECTS:frontierx_texture_library_owner>
	Tools/portability/PolyMeshFixture.cpp
	Tools/portability/game_material_manager_probe.cpp)
target_include_directories(frontierx_game_material_manager_probe PRIVATE
	Code/App/Src Code/App/Src/Include Code/App/DInclude
	Code/Libs/Include Code/Libs/Src/Include
	Code/Libs/Src/RendComp/PolyMesh Code/Libs/Src/TextureManager)
target_link_libraries(frontierx_game_material_manager_probe PRIVATE
	frontierx_native_app_globals
	frontierx_native_dacom_bootstrap frontierx_native_math_accessor
	frontierx_math frontierx_native_render_pipeline frontierx_native_render_primitive frontierx_platform
	frontierx_native_app_math frontierx_native_app_render_state)
target_compile_features(frontierx_game_material_manager_probe PRIVATE cxx_std_17)
target_compile_definitions(frontierx_game_material_manager_probe PRIVATE
	FRONTIERX_VERTEX_COLOR_SOURCE="${PROJECT_SOURCE_DIR}/Data/mat/shaders/VertexColor.fx")

# Execute the production StateMaterial owner through its DACOM factory and a
# cached indexed draw at the established renderer boundary.
add_executable(frontierx_state_material_probe
	$<TARGET_OBJECTS:frontierx_texture_library_owner>
	$<TARGET_OBJECTS:frontierx_polymesh_mesh_owner>
	Tools/portability/MaterialTextureFixture.cpp
	Tools/portability/PolyMeshFixture.cpp
	Code/Libs/Src/Shading/Materials/MaterialLibrary.cpp
	Code/Libs/Src/Shading/Materials/SimpleMaterial.cpp
	Code/Libs/Src/Shading/Materials/StateMaterial.cpp
	Code/Libs/Src/VertexBufferManager/VertexBufferManager.cpp
	Code/Libs/Src/VertexBufferManager/CopyVertexBufferDesc.cpp
	Tools/portability/state_material_probe.cpp)
target_include_directories(frontierx_state_material_probe PRIVATE
	Code/Libs/Include Code/Libs/Src/RendComp/PolyMesh)
target_link_libraries(frontierx_state_material_probe PRIVATE
	frontierx_native_dacom_bootstrap frontierx_native_math_accessor
	frontierx_native_render_primitive
	frontierx_native_render_pipeline frontierx_platform)
target_compile_features(frontierx_state_material_probe PRIVATE cxx_std_17)
# Legacy Shading IMaterial and CQ2 IMaterialManager's IMaterial are unrelated
# interfaces. Keep their RTTI distinct when both DLL closures share one ELF.
target_compile_definitions(frontierx_state_material_probe PRIVATE
	IMaterial=NativeLegacyMaterial)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(frontierx_state_material_probe PRIVATE
		${FRONTIERX_RB_HEADER_WARNS} -Wno-unused-parameter)
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(frontierx_state_material_probe PRIVATE
		-Wno-delete-non-virtual-dtor)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_compile_options(frontierx_state_material_probe PRIVATE
		-Wno-delete-abstract-non-virtual-dtor
		-Wno-delete-non-abstract-non-virtual-dtor)
endif()

# Execute the production vertex/index arena owner used by RenderBatcher.
add_executable(frontierx_bmpooljr_probe
	$<TARGET_OBJECTS:frontierx_bmpooljr_object>
	Tools/portability/bmpooljr_probe.cpp)
target_include_directories(frontierx_bmpooljr_probe PRIVATE
	Code/App/Src Code/App/Src/Include Code/Libs/Include)
target_compile_features(frontierx_bmpooljr_probe PRIVATE cxx_std_17)
frontierx_enable_strict_warnings(frontierx_bmpooljr_probe)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(frontierx_bmpooljr_probe PRIVATE ${FRONTIERX_RB_HEADER_WARNS})
endif()

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
		Code/Libs/Src/Platform/NativeWindowManager.cpp
		Code/Libs/Src/Platform/NativeStreamer.cpp)
	target_include_directories(frontierx_native_window_manager PUBLIC Code/Libs/Include)
	target_link_libraries(frontierx_native_window_manager
		PUBLIC frontierx_sdl3_platform PRIVATE PkgConfig::SDL3)
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
	if(FRONTIERX_BUILD_FFMPEG_AUDIO OR FRONTIERX_BUILD_FFMPEG_VIDEO)
		pkg_check_modules(FRONTIERX_AV REQUIRED IMPORTED_TARGET libavcodec>=63.1.101 libavformat>=63.1.101 libavutil>=61.1.101)
		foreach(library IN LISTS FRONTIERX_AV_LINK_LIBRARIES)
			if(library MATCHES "\\.a$")
				message(FATAL_ERROR "Native media requires replaceable shared FFmpeg libraries")
			endif()
		endforeach()
		include(CheckCXXSourceRuns)
		set(CMAKE_REQUIRED_LIBRARIES PkgConfig::FRONTIERX_AV)
		unset(FRONTIERX_FFMPEG_LGPL CACHE)
		check_cxx_source_runs([=[
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
#include <cstring>
int main() {
 return std::strcmp(avcodec_license(), "LGPL version 2.1 or later") ||
        std::strcmp(avformat_license(), "LGPL version 2.1 or later") ||
        std::strcmp(avutil_license(), "LGPL version 2.1 or later");
}
]=] FRONTIERX_FFMPEG_LGPL)
		unset(CMAKE_REQUIRED_LIBRARIES)
		if(NOT FRONTIERX_FFMPEG_LGPL)
			message(FATAL_ERROR "Native media requires a separately reviewed shared LGPL 2.1 FFmpeg build; GPL/nonfree builds are rejected")
		endif()
		target_link_libraries(frontierx_native_window_manager PRIVATE PkgConfig::FRONTIERX_AV)
	endif()
	if(FRONTIERX_BUILD_FFMPEG_AUDIO)
		target_sources(frontierx_native_window_manager PRIVATE Code/Libs/Src/Platform/NativeIacWave.cpp)
		target_compile_definitions(frontierx_native_window_manager PRIVATE FRONTIERX_HAVE_IAC_DECODER=1)
	endif()
	if(FRONTIERX_BUILD_FFMPEG_VIDEO)
		pkg_check_modules(FRONTIERX_SWSCALE REQUIRED IMPORTED_TARGET libswscale)
		foreach(library IN LISTS FRONTIERX_SWSCALE_LINK_LIBRARIES)
			if(library MATCHES "\\.a$")
				message(FATAL_ERROR "Native video requires replaceable shared libswscale")
			endif()
		endforeach()
		set(CMAKE_REQUIRED_LIBRARIES PkgConfig::FRONTIERX_SWSCALE)
		unset(FRONTIERX_SWSCALE_LGPL CACHE)
		check_cxx_source_runs([=[
extern "C" {
#include <libswscale/swscale.h>
}
#include <cstring>
int main() { return std::strcmp(swscale_license(), "LGPL version 2.1 or later"); }
]=] FRONTIERX_SWSCALE_LGPL)
		unset(CMAKE_REQUIRED_LIBRARIES)
		if(NOT FRONTIERX_SWSCALE_LGPL)
			message(FATAL_ERROR "Native video requires reviewed shared LGPL 2.1 libswscale")
		endif()
		target_sources(frontierx_native_window_manager PRIVATE Code/Libs/Src/Platform/NativeVideoSystem.cpp)
		target_compile_definitions(frontierx_native_window_manager PUBLIC FRONTIERX_HAVE_VIDEO_DECODER=1)
		target_link_libraries(frontierx_native_window_manager PRIVATE PkgConfig::FRONTIERX_SWSCALE)
	endif()
	add_executable(frontierx_native_shell Code/Launcher/NativeShell.cpp)
	if(FRONTIERX_BUILD_FFMPEG_VIDEO)
		add_library(frontierx_app_movie_screen_owner OBJECT Code/App/Src/MovieScreen.cpp)
		target_compile_definitions(frontierx_app_toolbar_owner PRIVATE FRONTIERX_HAVE_VIDEO_DECODER=1)
		target_include_directories(frontierx_app_movie_screen_owner PRIVATE
			Code/App/Src Code/App/Src/Include Code/App/DInclude Code/Libs/Include)
		target_compile_features(frontierx_app_movie_screen_owner PRIVATE cxx_std_17)
		target_link_libraries(frontierx_native_shell PRIVATE
			$<TARGET_OBJECTS:frontierx_app_movie_screen_owner>)
	endif()
	add_dependencies(frontierx_native_shell frontierx_event_system_module)
	target_link_libraries(frontierx_native_shell
		PRIVATE
			frontierx_native_app_globals
			frontierx_app_string_resource
			frontierx_native_search
			frontierx_native_in_progress_anim
			frontierx_native_app_version
			frontierx_native_app_text_resources
			frontierx_native_app_schema
			frontierx_native_music_data_format
			frontierx_dacom_core
			frontierx_event_system
			frontierx_hotkey_event
			frontierx_native_dacom_bootstrap
			frontierx_native_user_defaults
			frontierx_native_window_manager
			frontierx_view_constructor
			frontierx_platform
			PkgConfig::SDL3)
    if(FRONTIERX_BUILD_NATIVE_FONTS)
        target_link_libraries(frontierx_native_shell PRIVATE Freetype::Freetype)
    endif()
	target_sources(frontierx_native_shell PRIVATE
		$<TARGET_OBJECTS:frontierx_app_system_container_owner>
		$<TARGET_OBJECTS:frontierx_app_global_startup_owner>
		$<TARGET_OBJECTS:frontierx_app_cursor_owner>
		$<TARGET_OBJECTS:frontierx_app_movie_camera_manager_owner>
		$<TARGET_OBJECTS:frontierx_app_music_manager_owner>
		$<TARGET_OBJECTS:frontierx_native_sfx_owner>
		$<TARGET_OBJECTS:frontierx_native_app_profile_parser>
		$<TARGET_OBJECTS:frontierx_app_objwatch_owner>
		$<TARGET_OBJECTS:frontierx_native_system_map_owner>)
	target_link_libraries(frontierx_native_shell PRIVATE frontierx_script01_resources frontierx_script02_resources frontierx_script03_resources)
	target_compile_features(frontierx_native_shell PRIVATE cxx_std_17)
	frontierx_enable_strict_warnings(frontierx_native_shell)
	set_property(TARGET frontierx_native_shell PROPERTY ENABLE_EXPORTS ON)
	add_executable(frontierx_sdl3_window_probe Tools/portability/sdl3_window_probe.cpp)
	target_link_libraries(frontierx_sdl3_window_probe PRIVATE frontierx_sdl3_platform PkgConfig::SDL3)
	target_compile_features(frontierx_sdl3_window_probe PRIVATE cxx_std_17)
	frontierx_enable_strict_warnings(frontierx_sdl3_window_probe)
	if(FRONTIERX_BUILD_SDL3_GPU)
		target_sources(frontierx_native_shell PRIVATE
			$<TARGET_OBJECTS:frontierx_app_camera_owner>
			$<TARGET_OBJECTS:frontierx_app_modal_owner>
			$<TARGET_OBJECTS:frontierx_app_script_owner>
			$<TARGET_OBJECTS:frontierx_app_scripting_owner>
			$<TARGET_OBJECTS:frontierx_native_mission_media_owner>
			$<TARGET_OBJECTS:frontierx_app_string_data_owner>
			$<TARGET_OBJECTS:frontierx_app_nugget_owner>
			$<TARGET_OBJECTS:frontierx_app_unborn_mesh_owner>
			$<TARGET_OBJECTS:frontierx_app_comm_track_owner>
			$<TARGET_OBJECTS:frontierx_app_fog_owner>
			$<TARGET_OBJECTS:frontierx_app_terrain_map_owner>
			$<TARGET_OBJECTS:frontierx_app_sector_owner>
			$<TARGET_OBJECTS:frontierx_app_space_environment_owner>
			$<TARGET_OBJECTS:frontierx_app_vertex_buffer_owner>
			$<TARGET_OBJECTS:frontierx_app_mglobals_owner>
			$<TARGET_OBJECTS:frontierx_app_object_list_owner>
			$<TARGET_OBJECTS:frontierx_app_base_object_owner>
			$<TARGET_OBJECTS:frontierx_app_object_comm_owner>
			$<TARGET_OBJECTS:frontierx_app_ui_anim_owner>
			$<TARGET_OBJECTS:frontierx_app_anim_obj_owner>
			$<TARGET_OBJECTS:frontierx_app_fabricator_owner>
			$<TARGET_OBJECTS:frontierx_app_spaceship_owner>
			$<TARGET_OBJECTS:frontierx_app_banker_owner>
			$<TARGET_OBJECTS:frontierx_app_jumpgate_owner>
			$<TARGET_OBJECTS:frontierx_app_waypoint_owner>
			$<TARGET_OBJECTS:frontierx_app_trigger_owner>
			$<TARGET_OBJECTS:frontierx_app_gunboat_owner>
			$<TARGET_OBJECTS:frontierx_app_build_support_platform_owner>
			$<TARGET_OBJECTS:frontierx_app_planetoid_owner>
			$<TARGET_OBJECTS:frontierx_app_object_map_owner>
			$<TARGET_OBJECTS:frontierx_app_event_scheduler_owner>
			$<TARGET_OBJECTS:frontierx_app_mission_owner>
			$<TARGET_OBJECTS:frontierx_app_op_agent_owner>
			$<TARGET_OBJECTS:frontierx_app_net_buffer_owner>
			$<TARGET_OBJECTS:frontierx_app_net_packet_owner>
			$<TARGET_OBJECTS:frontierx_app_effect_player_owner>
			$<TARGET_OBJECTS:frontierx_app_field_manager_owner>
			$<TARGET_OBJECTS:frontierx_app_minefield_owner>
			$<TARGET_OBJECTS:frontierx_app_antimatter_owner>
			$<TARGET_OBJECTS:frontierx_app_blast_owner>
			$<TARGET_OBJECTS:frontierx_app_light_owner>
			$<TARGET_OBJECTS:frontierx_app_map_gen_owner>
			$<TARGET_OBJECTS:frontierx_app_morph_mesh_owner>
			$<TARGET_OBJECTS:frontierx_app_random_gen_owner>
			$<TARGET_OBJECTS:frontierx_app_anim2d_owner>
			$<TARGET_OBJECTS:frontierx_app_dust_owner>
			$<TARGET_OBJECTS:frontierx_app_grid_vector_owner>
			$<TARGET_OBJECTS:frontierx_app_circle_lines_owner>
			$<TARGET_OBJECTS:frontierx_native_player_ai_boundary>
			$<TARGET_OBJECTS:frontierx_sol_t_script>
			$<TARGET_OBJECTS:frontierx_script01_owner>
			$<TARGET_OBJECTS:frontierx_script02_owner>
			$<TARGET_OBJECTS:frontierx_script03_owner>
			$<TARGET_OBJECTS:frontierx_native_app_2d_owner>
			$<TARGET_OBJECTS:frontierx_app_primitive_builder_owner>
			$<TARGET_OBJECTS:frontierx_app_gen_data_owner>
			$<TARGET_OBJECTS:frontierx_particle_owner>
			$<TARGET_OBJECTS:frontierx_mesh_manager_owner>
			$<TARGET_OBJECTS:frontierx_animation_owner>
			$<TARGET_OBJECTS:frontierx_channel_owner>
			$<TARGET_OBJECTS:frontierx_native_animation_validation>
			$<TARGET_OBJECTS:frontierx_hardpoint_owner>
			$<TARGET_OBJECTS:frontierx_optics_owner>)
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
			frontierx_sol_t_string_resource
			frontierx_sol_t_schema_resource
			frontierx_native_global_data_format
			frontierx_native_app_globals
			frontierx_native_app_diagnostics
			frontierx_native_app_math
			frontierx_native_app_render_state
			frontierx_native_render_pipeline
			frontierx_native_dacom_bootstrap
			frontierx_tinyxml
			frontierx_math
			frontierx_native_render_primitive
			frontierx_native_math_accessor
			frontierx_render_smoke_scene
			frontierx_sdl3_gpu_renderer)
		target_link_libraries(frontierx_game_material_manager_probe PRIVATE
			frontierx_sdl3_gpu_renderer frontierx_render_smoke_scene PkgConfig::SDL3)
		target_compile_definitions(frontierx_game_material_manager_probe PRIVATE
			FRONTIERX_GAME_GPU=1)
		target_sources(frontierx_native_shell PRIVATE
			Code/Launcher/NativeGameSystem.cpp
			Code/Launcher/NativeGameScene.cpp
			$<TARGET_OBJECTS:frontierx_app_mesh_render_owner>
			$<TARGET_OBJECTS:frontierx_app_toolbar_owner>
			$<TARGET_OBJECTS:frontierx_game_material_manager_owner>
			$<TARGET_OBJECTS:frontierx_game_texture_manager_owner>
			$<TARGET_OBJECTS:frontierx_polymesh_mesh_owner>
			$<TARGET_OBJECTS:frontierx_polymesh_component_owner>
			$<TARGET_OBJECTS:frontierx_engine_owner>
			$<TARGET_OBJECTS:frontierx_render_manager_owner>
			$<TARGET_OBJECTS:frontierx_x86_math_owner>
			$<TARGET_OBJECTS:frontierx_light_manager_owner>
			$<TARGET_OBJECTS:frontierx_vertex_buffer_manager_owner>
			$<TARGET_OBJECTS:frontierx_string_table_owner>
			$<TARGET_OBJECTS:frontierx_texture_library_owner>
			$<TARGET_OBJECTS:frontierx_render_batcher_object>
			$<TARGET_OBJECTS:frontierx_bmpooljr_object>)
		set(frontierx_game_scene_header_warns
			${FRONTIERX_RB_HEADER_WARNS} -Wno-switch -Wno-ignored-qualifiers)
		if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
			list(APPEND frontierx_game_scene_header_warns -Wno-class-memaccess)
		endif()
		set_source_files_properties(Code/Launcher/NativeGameScene.cpp PROPERTIES
			COMPILE_OPTIONS "${frontierx_game_scene_header_warns}")
		set_source_files_properties(Code/Launcher/NativeShell.cpp PROPERTIES
			COMPILE_OPTIONS "${frontierx_game_scene_header_warns}")
		# The bridge includes the same legacy ENGINE.H contract as the scene owner.
		set_source_files_properties(Code/Launcher/NativeGameSystem.cpp PROPERTIES
			COMPILE_OPTIONS "${frontierx_game_scene_header_warns}")
		target_include_directories(frontierx_native_shell PRIVATE
			Code/App/Src Code/App/Src/Include Code/App/DInclude
			Code/Libs/Src/RendComp/PolyMesh Code/Libs/Src/TextureManager)
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
		set(FRONTIERX_GPU_T4_VERTEX_SHADER
			${FRONTIERX_GPU_SHADER_BINARY_DIR}/sdl3_gpu_probe_t4.vert.spv)
		set(FRONTIERX_GPU_MT_VERTEX_SHADER
			${FRONTIERX_GPU_SHADER_BINARY_DIR}/sdl3_gpu_probe_mt.vert.spv)
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
		add_custom_command(
			OUTPUT ${FRONTIERX_GPU_T4_VERTEX_SHADER}
			COMMAND ${CMAKE_COMMAND} -E make_directory ${FRONTIERX_GPU_SHADER_BINARY_DIR}
			COMMAND ${FRONTIERX_GLSLC_EXECUTABLE}
				--target-env=vulkan1.0
				-fshader-stage=vert
				-o ${FRONTIERX_GPU_T4_VERTEX_SHADER}
				${FRONTIERX_GPU_SHADER_SOURCE_DIR}/sdl3_gpu_probe_t4.vert
			DEPENDS ${FRONTIERX_GPU_SHADER_SOURCE_DIR}/sdl3_gpu_probe_t4.vert
			VERBATIM)
		add_custom_command(
			OUTPUT ${FRONTIERX_GPU_MT_VERTEX_SHADER}
			COMMAND ${CMAKE_COMMAND} -E make_directory ${FRONTIERX_GPU_SHADER_BINARY_DIR}
			COMMAND ${FRONTIERX_GLSLC_EXECUTABLE}
				--target-env=vulkan1.0
				-fshader-stage=vert
				-o ${FRONTIERX_GPU_MT_VERTEX_SHADER}
				${FRONTIERX_GPU_SHADER_SOURCE_DIR}/sdl3_gpu_probe_mt.vert
			DEPENDS ${FRONTIERX_GPU_SHADER_SOURCE_DIR}/sdl3_gpu_probe_mt.vert
			VERBATIM)
		add_custom_target(frontierx_sdl3_gpu_shaders DEPENDS
			${FRONTIERX_GPU_VERTEX_SHADER}
			${FRONTIERX_GPU_FRAGMENT_SHADER}
			${FRONTIERX_GPU_T4_VERTEX_SHADER}
			${FRONTIERX_GPU_MT_VERTEX_SHADER})
		add_dependencies(frontierx_native_shell frontierx_sdl3_gpu_shaders)
		add_dependencies(frontierx_game_material_manager_probe
			frontierx_sdl3_gpu_shaders)
		add_executable(frontierx_sdl3_gpu_probe
			$<TARGET_OBJECTS:frontierx_texture_library_owner>
			Tools/portability/MaterialTextureFixture.cpp
			Tools/portability/sdl3_gpu_probe.cpp
			Code/Libs/Src/Shading/Materials/MaterialLibrary.cpp
			Code/Libs/Src/Shading/Materials/SimpleMaterial.cpp
			Code/Libs/Src/Shading/Materials/StateMaterial.cpp
			Code/Libs/Src/VertexBufferManager/VertexBufferManager.cpp
			Code/Libs/Src/VertexBufferManager/CopyVertexBufferDesc.cpp)
		add_dependencies(frontierx_sdl3_gpu_probe frontierx_sdl3_gpu_shaders)
		target_link_libraries(frontierx_sdl3_gpu_probe
			PRIVATE
				frontierx_native_dacom_bootstrap
				frontierx_native_render_primitive
				frontierx_native_render_pipeline
				frontierx_platform
				frontierx_render_smoke_scene
				frontierx_sdl3_gpu_renderer
				PkgConfig::SDL3)
		target_compile_features(frontierx_sdl3_gpu_probe PRIVATE cxx_std_17)
		target_compile_definitions(frontierx_sdl3_gpu_probe PRIVATE
			IMaterial=NativeLegacyMaterial)
		frontierx_enable_strict_warnings(frontierx_sdl3_gpu_probe)
		if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
			target_compile_options(frontierx_sdl3_gpu_probe PRIVATE
				${FRONTIERX_RB_HEADER_WARNS} -Wno-unused-parameter)
		endif()
		if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
			target_compile_options(frontierx_sdl3_gpu_probe PRIVATE
				-Wno-delete-non-virtual-dtor -Wno-write-strings)
		elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
			target_compile_options(frontierx_sdl3_gpu_probe PRIVATE
				-Wno-delete-abstract-non-virtual-dtor
				-Wno-delete-non-abstract-non-virtual-dtor
				-Wno-writable-strings)
		endif()
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
		PRIVATE frontierx_platform
			frontierx_dacom_core
			frontierx_native_window_manager
			PkgConfig::SDL3)
	if(FRONTIERX_BUILD_FFMPEG_VIDEO)
		target_link_libraries(frontierx_native_window_manager_probe PRIVATE
			frontierx_native_render_pipeline frontierx_native_render_primitive)
	endif()
	target_compile_features(frontierx_native_window_manager_probe PRIVATE cxx_std_17)
	frontierx_enable_strict_warnings(frontierx_native_window_manager_probe)
	if(FRONTIERX_BUILD_FFMPEG_AUDIO)
		target_compile_definitions(frontierx_native_window_manager_probe PRIVATE FRONTIERX_HAVE_IAC_DECODER=1)
		target_include_directories(frontierx_native_window_manager_probe PRIVATE Code/Libs/Src/Platform)
	endif()
endif()

include(CTest)
# M3 preflight: actual CRT draws used by simulation owners, not a simulation.
add_executable(frontierx_rng_contract_probe Tools/portability/rng_contract_probe.cpp
	$<TARGET_OBJECTS:frontierx_app_random_gen_owner>)
target_include_directories(frontierx_rng_contract_probe PRIVATE
	Code/App/Src Code/Libs/Include)
frontierx_enable_strict_warnings(frontierx_rng_contract_probe)

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
	add_test(NAME portability.rng_contract COMMAND frontierx_rng_contract_probe)
	add_test(NAME portability.m3_ci_gates COMMAND ${Python3_EXECUTABLE}
		${CMAKE_CURRENT_SOURCE_DIR}/Tools/ci/test_ci_gates.py)
	add_test(NAME portability.m3_compare COMMAND ${Python3_EXECUTABLE}
		${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability/test_m3_compare.py)
	add_test(NAME portability.layout COMMAND frontierx_layout_probe)
	add_test(NAME portability.tinyxml COMMAND frontierx_tinyxml_probe)
	add_test(NAME portability.string_table_file_text
		COMMAND frontierx_string_table_file_text_probe)
	add_test(NAME portability.string_table_owner
		COMMAND frontierx_string_table_owner_probe
			${CMAKE_CURRENT_BINARY_DIR}/string-table-owner-fixture.utf)
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
	add_test(NAME portability.view_parser COMMAND frontierx_view_parser_probe)
	add_test(NAME portability.app_schema COMMAND frontierx_view_parser_probe
		--app-schema ${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/data.i)
	add_test(NAME portability.script03_schema COMMAND frontierx_view_parser_probe
		--third-terran-script-schema ${frontierx_script03_schema})
	add_test(NAME portability.script02_schema COMMAND frontierx_view_parser_probe
		--second-terran-script-schema ${frontierx_script02_schema})
	add_test(NAME portability.script01_schema COMMAND frontierx_view_parser_probe
		--terran-script-schema ${frontierx_script01_schema})
	add_test(NAME portability.sol_t_schema COMMAND frontierx_view_parser_probe
		--script-schema ${frontierx_sol_t_schema})
	add_test(NAME portability.light_manager COMMAND frontierx_light_manager_probe)
	add_test(NAME portability.preference_store COMMAND frontierx_preference_store_probe)
	add_test(NAME portability.native_user_settings COMMAND frontierx_native_user_settings_probe)
	add_test(NAME portability.native_user_defaults COMMAND frontierx_native_user_defaults_probe)
	add_test(NAME portability.encoded_handle COMMAND frontierx_encoded_handle_probe)
	add_test(NAME portability.dacom_runtime COMMAND frontierx_dacom_runtime_probe)
	add_test(NAME portability.dacom_interface COMMAND frontierx_dacom_interface_probe)
	add_test(NAME portability.app_global_startup COMMAND frontierx_app_global_startup_probe)
	add_test(NAME portability.app_cursor COMMAND frontierx_app_cursor_probe)
	set_tests_properties(portability.app_cursor PROPERTIES SKIP_RETURN_CODE 77 LABELS "host")
	add_test(NAME portability.net_buffer COMMAND frontierx_net_buffer_probe
		${CMAKE_CURRENT_BINARY_DIR}/net-buffer-probe-defaults)
	add_test(NAME portability.stack_address COMMAND frontierx_stack_address_probe)
	add_test(NAME portability.particle_context COMMAND frontierx_particle_context_probe)
	add_test(NAME portability.math_core COMMAND frontierx_math_core_probe)
	add_test(NAME portability.ray_geometry COMMAND frontierx_ray_geometry_probe)
	add_test(NAME portability.render_submission COMMAND frontierx_render_submission_probe)
	add_test(NAME portability.render_pipeline COMMAND frontierx_render_pipeline_probe)
	add_test(NAME portability.render_batcher_header COMMAND frontierx_render_batcher_header_probe)
	add_test(NAME portability.vertex_buffer_manager COMMAND frontierx_vertex_buffer_manager_probe)
	add_test(NAME portability.state_material COMMAND frontierx_state_material_probe
		${CMAKE_CURRENT_BINARY_DIR}/material_library_fixture.utf)
	add_test(NAME portability.game_material_manager COMMAND frontierx_game_material_manager_probe
		${CMAKE_CURRENT_BINARY_DIR}/game_material_manager_fixture.utf)
	add_test(NAME portability.game_material_manager_cr COMMAND frontierx_game_material_manager_probe
		${CMAKE_CURRENT_BINARY_DIR}/game_material_manager_cr_fixture.utf cr)
	add_test(NAME portability.game_material_manager_missing_end COMMAND frontierx_game_material_manager_probe
		${CMAKE_CURRENT_BINARY_DIR}/game_material_manager_missing_end_fixture.utf missing-end)
	add_test(NAME portability.math_accessor COMMAND frontierx_math_accessor_probe)
	add_test(NAME portability.render_batcher_runtime COMMAND frontierx_render_batcher_runtime_probe)
	add_test(NAME portability.bmpooljr COMMAND frontierx_bmpooljr_probe)
	if(TARGET frontierx_sdl3_window_probe)
		add_test(NAME portability.sdl3_window COMMAND frontierx_sdl3_window_probe)
		set_tests_properties(portability.sdl3_window PROPERTIES ENVIRONMENT "SDL_VIDEODRIVER=dummy")
		add_test(NAME portability.legacy_input COMMAND frontierx_legacy_input_probe)
		set_tests_properties(portability.legacy_input PROPERTIES ENVIRONMENT "SDL_VIDEODRIVER=dummy")
		add_test(NAME portability.native_window_manager COMMAND frontierx_native_window_manager_probe)
		set_tests_properties(portability.native_window_manager PROPERTIES
			ENVIRONMENT "SDL_VIDEODRIVER=dummy;SDL_AUDIODRIVER=dummy")
		add_test(NAME portability.native_shell COMMAND ${Python3_EXECUTABLE}
			${CMAKE_CURRENT_SOURCE_DIR}/Tools/portability/test_native_shell.py
			$<TARGET_FILE:frontierx_native_shell>
			$<TARGET_FILE:frontierx_event_system_module>
			${CMAKE_CURRENT_SOURCE_DIR}/Code/App/Src/Hotkeys.utf
			$<BOOL:${FRONTIERX_BUILD_SDL3_GPU}> "${FRONTIERX_TEST_FONT_FILE}"
			$<BOOL:${FRONTIERX_BUILD_FFMPEG_VIDEO}>)
	endif()
	if(TARGET frontierx_sdl3_gpu_probe)
		# Existing production closure: Engine -> RenderMgr -> PolyMesh -> SDL GPU.
		# The regular material probe cannot establish visible runtime execution.
		add_test(NAME portability.game_material_gpu COMMAND frontierx_game_material_manager_probe
			${CMAKE_CURRENT_BINARY_DIR}/game_material_gpu_fixture.utf
			--gpu ${FRONTIERX_GPU_VERTEX_SHADER}
			${FRONTIERX_GPU_FRAGMENT_SHADER}
			${FRONTIERX_GPU_T4_VERTEX_SHADER}
			${FRONTIERX_GPU_MT_VERTEX_SHADER})
		set_tests_properties(portability.game_material_gpu PROPERTIES SKIP_RETURN_CODE 77 LABELS "gpu;host")
		add_test(NAME portability.sdl3_gpu COMMAND frontierx_sdl3_gpu_probe
			${FRONTIERX_GPU_VERTEX_SHADER}
			${FRONTIERX_GPU_FRAGMENT_SHADER}
			${FRONTIERX_GPU_T4_VERTEX_SHADER}
			${CMAKE_CURRENT_BINARY_DIR}/sdl_gpu_material_texture_fixture.utf
			${FRONTIERX_GPU_MT_VERTEX_SHADER}
			--visible)
		set_tests_properties(portability.sdl3_gpu PROPERTIES SKIP_RETURN_CODE 77 LABELS "gpu;host")
	endif()
endif()
