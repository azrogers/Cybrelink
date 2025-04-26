# CMake file adapted from
# https://github.com/CesiumGS/cesium-native/blob/main/CMakeLists.txt, used under
# Apache license

cmake_minimum_required(VERSION 3.30)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
    set(VCPKG_TARGET_TRIPLET x64-windows-static)
elseif(APPLE)
    set(VCPKG_TARGET_TRIPLET x64-osx-static)
else()
    set(VCPKG_TARGET_TRIPLET x64-linux-static)
endif(WIN32)

file(TO_CMAKE_PATH "$ENV{VCPKG_ROOT}" ENV_VCPKG_ROOT)
set(CMAKE_TOOLCHAIN_FILE "${ENV_VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()
set(CMAKE_CXX_STANDARD 20)

project(
    uplink
    VERSION 10.0
    LANGUAGES CXX C)

set(UPLINK_DEBUG_POSTFIX "d")
set(UPLINK_RELEASE_POSTFIX "")

set(CMAKE_DEBUG_POSTFIX ${UPLINK_DEBUG_POSTFIX})
set(CMAKE_RELEASE_POSTFIX ${UPLINK_RELEASE_POSTFIX})
set(CMAKE_MINSIZEREL_POSTFIX ${UPLINK_RELEASE_POSTFIX})
set(CMAKE_RELWITHDEBINFO_POSTFIX ${UPLINK_RELEASE_POSTFIX})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/uplink/bin)

if(WIN32)
	include(${CMAKE_CURRENT_LIST_DIR}/cmake/FindWindowsSDK.cmake)
	if (NOT WINDOWSSDK_FOUND)
		message(FATAL_ERROR "Windows SDK not found")
	endif()

	get_windowssdk_include_dirs(${WINDOWSSDK_LATEST_DIR} WINDOWSSDK_INCLUDE_DIRS)
endif()

function(uplink_glob_files out_var_name regexes)
    set(files "")
    foreach(arg ${ARGV})
        list(APPEND regexes_only "${arg}")
    endforeach()
    list(POP_FRONT regexes_only)
    if(GLOB_USE_CONFIGURE_DEPENDS)
        file(GLOB_RECURSE files CONFIGURE_DEPENDS ${regexes_only})
    else()
        file(GLOB files ${regexes_only})
    endif()
    set(${ARGV0}
        "${files}"
        PARENT_SCOPE)
endfunction()
add_definitions(-w)
function(configure_uplink_component targetName)
    set_target_properties(
        ${targetName}
        PROPERTIES CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)

    target_compile_definitions(
        ${targetName}
        PRIVATE
        $<IF:$<CONFIG:Debug>,VERSION_NAME_INT="DEBUG",VERSION_NAME_INT="RELEASE">
        VERSION_NUMBER_INT="${CMAKE_PROJECT_VERSION}" WIN32_LEAN_AND_MEAN)

    if(WIN32)
        target_compile_definitions(
            ${targetName}
            PRIVATE
            WIN32
            _CRT_SECURE_NO_WARNINGS)
    endif()

	target_include_directories(${targetName} PRIVATE ${WINDOWSSDK_INCLUDE_DIRS})

    if(MSVC)
        target_compile_options(${targetName} PRIVATE /wd4477)
    endif()
endfunction()

add_subdirectory(lib)
add_subdirectory(uplink)

# clang-format target

include(${CMAKE_CURRENT_LIST_DIR}/cmake/CPM.cmake)
cpmaddpackage(
    NAME
    Format.cmake
    VERSION
    1.7.3
    GITHUB_REPOSITORY
    TheLartians/Format.cmake
    OPTIONS
    # set to yes skip cmake formatting
    "FORMAT_SKIP_CMAKE NO"
    # set to yes skip clang formatting
    "FORMAT_SKIP_CLANG NO"
    # path to exclude (optional, supports regular expressions)
    "CMAKE_FORMAT_EXCLUDE cmake/CPM.cmake")
