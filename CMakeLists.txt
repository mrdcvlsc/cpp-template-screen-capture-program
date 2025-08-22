cmake_minimum_required(VERSION 3.30)
project(ScreenShotApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(ExternalProject)

# download stb_image_write.h from the official repository
ExternalProject_Add(
    stb_image_write
    URL https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h
    DOWNLOAD_NO_EXTRACT 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/external/stb && ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/_deps/stb_image_write/src/stb_image_write.h ${CMAKE_SOURCE_DIR}/external/stb/stb_image_write.h
    PREFIX ${CMAKE_BINARY_DIR}/_deps/stb_image_write
)

# ensure the directory exists for the header
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/external/stb)

ExternalProject_Add(
    screen_capture_lite
    GIT_REPOSITORY https://github.com/smasherprog/screen_capture_lite.git
    GIT_TAG master
    UPDATE_DISCONNECTED 1
    CMAKE_ARGS
        -DBUILD_SHARED_LIBS=OFF
        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/external
    PREFIX ${CMAKE_BINARY_DIR}/_deps/screen_capture_lite
    INSTALL_COMMAND ""
    BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --target screen_capture_lite_static
)

# do not set INTERFACE_INCLUDE_DIRECTORIES on the imported target, as the include directory may not exist at configure time
set(SCREEN_CAPTURE_LITE_INCLUDE_DIR ${CMAKE_BINARY_DIR}/_deps/screen_capture_lite/src/screen_capture_lite/include)
set(SCREEN_CAPTURE_LITE_LIB_PATH ${CMAKE_BINARY_DIR}/_deps/screen_capture_lite/src/screen_capture_lite-build/src_cpp/libscreen_capture_lite_static.a)

add_library(screen_capture_lite_static IMPORTED STATIC)
set_target_properties(screen_capture_lite_static PROPERTIES
    IMPORTED_LOCATION ${SCREEN_CAPTURE_LITE_LIB_PATH}
)
add_dependencies(screen_capture_lite_static screen_capture_lite)

add_executable(ScreenShotApp main.cpp)
add_dependencies(ScreenShotApp screen_capture_lite)
target_include_directories(ScreenShotApp PRIVATE ${SCREEN_CAPTURE_LITE_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/external/stb)
if(WIN32)
    target_link_libraries(ScreenShotApp PRIVATE
        screen_capture_lite_static
        Dwmapi
        Gdi32
    )
elif(APPLE)
    find_library(COREGRAPHICS_FRAMEWORK CoreGraphics)
    find_library(COREFND_FRAMEWORK CoreFoundation)
    target_link_libraries(ScreenShotApp PRIVATE
        screen_capture_lite_static
        ${COREGRAPHICS_FRAMEWORK}
        ${COREFND_FRAMEWORK}
    )
else()
    target_link_libraries(ScreenShotApp PRIVATE
        screen_capture_lite_static
        X11 Xfixes Xtst Xinerama Xext
    )
endif()
