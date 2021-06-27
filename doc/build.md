# Compiling *Wagic: War of the Homebrew*

WotH's source code is currently hosted on [Github][0].

You might want to compile the game from the sources for various reasons. You want to play the game with the newest features which aren't present in the official release yet, or perhaps you want to add features to the game, or you're trying to port Wagic to another platform, or trying to create a Pokemon card game, who knows ...

Compiling *WotH* shouldn't be difficult, however you might need to some knowledge about the command line of your particular system. The tricky part is to provide the build dependencies.

To compile *WotH* you need to provide:
* a C++ compiler with C++17 support (e.g. *gcc* or *clang*)
* [CMake](https://cmake.org/) as build system
* [zlib](https://github.com/madler/zlib) for zip file support
* [FreeType](https://www.freetype.org/index.html) for true type fonts (TTF) support
* [libpng](http://www.libpng.org/pub/png/libpng.html) and [libjpeg](https://www.libjpeg-turbo.org/) for image handling
* a platform layer or SDK
  * *PSP*: [PSPSDK][5]
  * *Linux/Window*: [SDL2](https://www.libsdl.org/index.php)

[0]: https://github.com/zie87/wth_woth

# Compiling for Windows

1. Download and install Microsoft's [Visual Studio 2019 (Community)](https://visualstudio.microsoft.com/vs/community/) 
2. Download and install [CMake][2]
3. Download and install Microsoft's [vcpkg][1] according to the [Quick Start Guide](https://github.com/Microsoft/vcpkg#quick-start-windows)
    ```
    git clone https://github.com/microsoft/vcpkg
    .\vcpkg\bootstrap-vcpkg.bat
    ```
4. Install the dependencies via [vcpkg][1]. Currently only 32 Bit builds are supported.
    ```
    .\vcpkg\vcpkg install sdl2 zlib libpng libjpeg-turbo freetype --triplet x86-windows
    ```
5. Download the [source code][0] from *git*:
    ```
    git clone --recursive https://github.com/zie87/wth_woth wth_woth
    ```
6. Start the configuration process via [CMake][2]. We need to define *Visual Studio 2019* as generater (`-G "Visual Studio 16 2019"`), Win32 as architecture (`-A Win32`), the build type (`Debug` or `Release`) and final the toolchain file provided by [vcpkg][1] (`-DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake`). The complete command will look similar to the following:
    ```
    cmake -S . -B build -G "Visual Studio 16 2019" -A Win32 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake
    ```
7. Start the compiling of the source code via [CMake][2] with this command:
    ```
    cmake --build build --config Debug
    ```

[1]: https://vcpkg.io/en/index.html
[2]: https://cmake.org/download/

# Compiling for Linux

1. Download and install the build dependencies
    ```
    # Ubuntu
    sudo apt update
    sudo apt install build-essential g++ cmake git libsdl2-dev libpng-dev libjpeg-dev libfreetype-dev libz-dev
    ```
    ```
    # Arch Linux
    sudo pacman -Sy
    sudo base-devel pacman -S cmake gcc git sdl2 libpng libjpeg-turbo freetype2 zlib
    ```
2. Download the [source code][0] from *git*:
    ```
    git clone --recursive https://github.com/zie87/wth_woth wth_woth
    ```
3. Start the configuration process via [CMake][2]. We need *only* to define the build type (`Debug` or `Release`).
    ```
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
    ```
4. Start the compiling of the source code via [CMake][2] with this command:
    ```
    cmake --build build -- -j $(nproc)
    ```

[3]: https://ubuntu.com/
[4]: https://archlinux.org/

# Compiling for PSP

To run *WotH* on the PSP, you need to compile a new **EBOOT.PBP**. For the compiling the tools provided by [PSPSDK][5] are used. The installation of the [SDK][5] can be cumbersome. A [docker](https://www.docker.com/) image is provided to minimize the afford: [docker image][6]

[5]: https://github.com/pspdev/pspsdk
[6]: https://hub.docker.com/repository/docker/zie87/wth_woth_pspdev

## using Docker

1. Install docker accoring to your host platform:
    * Windows: [Install Docker Desktop on Windows](https://docs.docker.com/docker-for-windows/install/)
    * [Ubuntu][3]: `sudo apt update && sudo apt install docker`
    * [Arch][4]: `sudo pacman -Sy && sudo pacman -S docker`
2. Pull the [docker image][6]:
    ```
    docker pull zie87/wth_woth_pspdev:main
    ```
3. Download the [source code][0] from *git*:
    ```
    git clone --recursive https://github.com/zie87/wth_woth wth_woth
    ```
4. Build the source code inside the Docker Container. A script is provided which will start the build process. We only need to start the script inside the container:
    ```
    docker run --rm -u $UID --init -v $PWD:/opt/workspace -e LANG=$LANG -w /opt/workspace -it zie87/wth_woth_pspdev /bin/bash "./.github/tools/psp/build.sh" Debug
    ```