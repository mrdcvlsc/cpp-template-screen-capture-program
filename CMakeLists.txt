cmake_minimum_required(VERSION 3.30)
project(ScreenShotApp LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_VERBOSE_MAKEFILE OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(ExternalProject)

# ================ download stb_image_write.h ================

ExternalProject_Add(
    stb_image_write
    URL https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h
    DOWNLOAD_NO_EXTRACT 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/external/stb && ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/_deps/stb_image_write/src/stb_image_write.h ${CMAKE_SOURCE_DIR}/external/stb/stb_image_write.h
    PREFIX ${CMAKE_BINARY_DIR}/_deps/stb_image_write
)

file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/external/stb) # ensure the directory exists for the header

# ================ download screen_capture_lite ================

ExternalProject_Add(
    screen_capture_lite
    GIT_REPOSITORY https://github.com/smasherprog/screen_capture_lite.git
    GIT_TAG master
    UPDATE_DISCONNECTED 1
    CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=Release
        -DBUILD_SHARED_LIBS=OFF
        -DBUILD_EXAMPLE=OFF
        -DBUILD_CSHARP=OFF
        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/external
    PREFIX ${CMAKE_BINARY_DIR}/_deps/screen_capture_lite
    INSTALL_COMMAND ""
    BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config Release --target screen_capture_lite_static
)

set(SCREEN_CAPTURE_LITE_INCLUDE_DIR ${CMAKE_BINARY_DIR}/_deps/screen_capture_lite/src/screen_capture_lite/include)

if(MSVC)
    set(SCREEN_CAPTURE_LITE_LIB_PATH ${CMAKE_BINARY_DIR}/_deps/screen_capture_lite/src/screen_capture_lite-build/src_cpp/Release/${CMAKE_STATIC_LIBRARY_PREFIX}screen_capture_lite_static${CMAKE_STATIC_LIBRARY_SUFFIX})
else()
    set(SCREEN_CAPTURE_LITE_LIB_PATH ${CMAKE_BINARY_DIR}/_deps/screen_capture_lite/src/screen_capture_lite-build/src_cpp/${CMAKE_STATIC_LIBRARY_PREFIX}screen_capture_lite_static${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()

add_library(screen_capture_lite_static IMPORTED STATIC)

set_target_properties(screen_capture_lite_static PROPERTIES
    IMPORTED_LOCATION ${SCREEN_CAPTURE_LITE_LIB_PATH}
)

add_dependencies(screen_capture_lite_static screen_capture_lite)

# setup screen capture static linked libraries

if(WIN32)
    add_definitions(-DNOMINMAX)
    set(SCREEN_CAPTURE_LITE_STATIC_LIB_AND_DEPENDENCIES
        screen_capture_lite_static
        Dwmapi Gdi32 Gdi32 d3d11.lib d3d12.lib dxgi.lib
    )
elseif(APPLE)
    find_library(corefoundation_lib CoreFoundation REQUIRED)
    find_library(cocoa_lib Cocoa REQUIRED)
    find_library(coremedia_lib CoreMedia REQUIRED)
    find_library(avfoundation_lib AVFoundation REQUIRED)
    find_library(coregraphics_lib CoreGraphics REQUIRED)
    find_library(corevideo_lib CoreVideo REQUIRED)
   
	set(SCREEN_CAPTURE_LITE_STATIC_LIB_AND_DEPENDENCIES
        screen_capture_lite_static
        ${CMAKE_THREAD_LIBS_INIT} ${corefoundation_lib}
        ${cocoa_lib} ${coremedia_lib} ${avfoundation_lib}
        ${coregraphics_lib} ${corevideo_lib}
    ) 
else()
    set(SCREEN_CAPTURE_LITE_STATIC_LIB_AND_DEPENDENCIES
        screen_capture_lite_static
        X11 Xfixes Xtst Xinerama Xext
    )
endif()

# ================ compile program ================

add_executable(ScreenShotApp main.cpp)
target_include_directories(ScreenShotApp PRIVATE ${SCREEN_CAPTURE_LITE_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/external/stb)
target_link_libraries(ScreenShotApp PRIVATE ${SCREEN_CAPTURE_LITE_STATIC_LIB_AND_DEPENDENCIES})
