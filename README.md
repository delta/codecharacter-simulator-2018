# Code Character Simulator

## Setup Instructions

1. Download [Boost 1.64](https://dl.bintray.com/boostorg/release/1.64.0/source/)

2. `git clone --recursive https://github.com/delta/codecharacter-simulator.git`

3. Install [CMake](https://cmake.org/download/) (> v3.9.6)

4. Install [Protobuf](https://github.com/google/protobuf/releases)

5. `mkdir build && cd build`

6. `export LD_LIBRARY_PATH=<your_install_location>/lib`

7. `BOOST_PATH=<boost_download_location> cmake .. -DCMAKE_INSTALL_PREFIX=<your_install_location>`

8. `make`

9. `make install`

10. To run the simulator, `<your_install_location>/bin/simulator`

To run the unit tests, `<your_install_location>/bin/test`

Pass `-DBUILD_PROJECT=<project_name>` to cmake to build only a specific module. Passing `no_tests` as the project name builds everything but the unit tests.
