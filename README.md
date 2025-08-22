# ScreenShotApp

This project demonstrates how to use the Screen Capture Lite library in a C++ application.

## Prerequisites

- C++17 compatible compiler (e.g., GCC 7+, Clang 6+, MSVC 2017+)
- CMake 3.10 or higher
- Git
- Linux: X11 development libraries (e.g., `libx11-dev`, `libxfixes-dev`, `libxtst-dev`, `libxinerama-dev`)
- `stb_image` for converting to png (automatically downloaded by cmake config)

## Clone the Repository

Clone the repository as usual:

```
git clone <repository-url>
cd scl-cmake-template
```

## Build Instructions

1. Run CMake to configure the project (initial run needs internet connection):
    ```sh
    cmake -S . -B build -DBUILD_CSHARP=OFF
    ```
2. Build the project:
    ```sh
    cmake --build build --config Release
    ```
## Run the Application

After building, you will find the executable in the `build` directory:

```
./build/ScreenShotApp
```

## Notes
- If you encounter errors related to missing X11 libraries, install the required development packages for your distribution.

- For more information, see the documentation in the Screen Capture Lite repository: https://github.com/smasherprog/screen_capture_lite