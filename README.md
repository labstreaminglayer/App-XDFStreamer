# XDF Streamer

XDF Streamer is a Qt C++ application that streams signals from [XDF](https://github.com/sccn/xdf) files to [lab streaming layer](https://github.com/sccn/labstreaminglayer).

![XDF Streamer](XdfStreamer.png "XDF Streamer")

## Download

You can find prebuilt distributions of XdfStreamer on [the releases page](https://github.com/labstreaminglayer/App-XDFStreamer/releases).

## Requirements

- __Qt__ >= 5.11
- [__liblsl__](https://github.com/sccn/liblsl/releases)
- [__libxdf__](https://github.com/xdf-modules/libxdf/releases)

## Build Instructions

If the dependencies are not installed to standard system folders then you will need to tell CMake where to find them. Add one or more of the following CMake arguments as needed:
* -DQt5_DIR=/volume/Qt/{platform}/lib/cmake/Qt5 
* -DLSL_INSTALL_ROOT=path/to/liblsl/unpack/
* -DXDF_INSTALL_ROOT=path/to/xdf/unpack/

If you are debugging or not intending to install to the system, then you need to change the install dir:
* -DCMAKE_INSTALL_PREFIX=${PWD}/build/install
