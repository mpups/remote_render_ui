# Remote User Interface

![Screenshot of User Interface](images/screenshot.png "Screenshot of UI.")

This is a prototype remote user-interface that connects to a rendering server running on a remote machine.

It has been tested on Mac OSX 11.6 and Ubuntu 18. It may also work on other platforms
where the dependencies can be satisfied.

### Dependencies

The following are required system dependencies:
- CMake >3.5 <=4.0 (install a specific version via `pip install` if your system defaults to an incompatible version).
- Ninja build system for CMake (optional)
- Boost (only tested with 1.78).
- FFmpeg (See README.md in the [videolib submodule](https://github.com/markp-gc/videolib) for instructions and version compatibilty).
- OpenCV
- Ubuntu only: GLFW
- Mac OSX only: Xcode with metal support
- E.g. to install Ubuntu dependencies:
  - sudo apt install libglfw3-dev libboost-dev libboost-log-dev libboost-program-options-dev cmake ninja-build libopencv-dev libxinerama-dev libxcursor-dev libxi-dev libboost-test-dev
- E.g. using brew on Mac OSX:
  - brew install boost cmake ninja opencv

The following dependencies are cloned and built as submodules (so do not need to be installed manually):
- [nanogui](https://github.com/mitsuba-renderer/nanogui): a cross platform toolkit for simple GUI applications.
- [packetcomms](https://github.com/mpups/packetcomms): a simple TCP/IP communication protocol designed for low latency.
- [videolib](https://github.com/markp-gc/videolib): A wrapper for FFmpeg that supports TCP video streaming using packetcomms.


#### Install Dependencies

On Ubuntu (apt):
```shell
sudo apt update
sudo apt install libglfw3-dev libboost-dev libboost-log-dev libboost-program-options-dev cmake ninja-build libopencv-dev libxinerama-dev libxcursor-dev libxi-dev libboost-test-dev nasm libx264-dev pkg-config
```

On Windows (mingw, msys2 pacman):
```shell
pacman -S mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-glew mingw-w64-ucrt-x86_64-glfw mingw-w64-x86_64-boost mingw-w64-ucrt-x86_64-opencv mingw-w64-ucrt-x86_64-libx264 nasm mingw-w64-ucrt-x86_64-python-virtualenv python-pip
```

Python (create a venv):
```shell
pip install torch torchvision torchaudio --index-url https://download.pytorch.org/whl/cu128
pip install nanobind opencv-python diffusers sentencepiece accelerate
```

#### Build instructions

With the dependencies above installed, run the following to perform a complete configuration and build from scratch:

```shell
git clone --recursive https://github.com/graphcore-research/remote-gen-ui.git
cd remote-gen-ui/
mkdir remote_render_ui/build
cd remote_render_ui/build

cd ../external/FFmpeg/
mkdir ../install
./configure --enable-shared --enable-libx264 --enable-gpl --disable-programs --enable-rpath --prefix=`realpath ../install`
make -j64
make install

cd ../../
mkdir build
cd build
PKG_CONFIG_PATH=`realpath ../external/install/lib/pkgconfig/` cmake -G Ninja ..
ninja -j64
```

#### Run

Typically you will only run the client application on your local machine and the server runs on a remote machine.

1. Launch the server:

```shell
PYTHONPATH=./ python ../python/run_server.py --port 4000
```
2. Wait until it logs that it is waiting for connection.
  - E.g.: `[12:26:06.674469] [I] [51157] User interface server listening on port 4000`

3. Launch the client and connect to the same port:
  - (Run with `--help` for a full list of options).

```shell
./remote-ui --host localhost --port 4000
```
