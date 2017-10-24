# Code Character Simulator

[![Build Status](https://travis-ci.com/delta/codecharacter-simulator.svg?token=AbNxYBvow6wszNAonGy5&branch=dev)](https://travis-ci.com/delta/codecharacter-simulator)

## Setup Instructions

1. Download [Boost 1.64](https://dl.bintray.com/boostorg/release/1.64.0/source/)

1. `git clone --recursive https://github.com/delta/codecharacter-simulator.git`

2. Install [CMake](https://cmake.org/download/) (> v3.6.2)

3. `mkdir build && cd build`

4. `export LD_LIBRARY_PATH=<your_install_location>/lib`

5. `BOOST_PATH=<boost_download_location> cmake .. -DCMAKE_INSTALL_PREFIX=<your_install_location>`

6. `make`

7. `make install`

8. To run the simulator, `<your_install_location>/bin/simulator`

To run the unit tests, `<your_install_location>/bin/test`

Pass `-DBUILD_PROJECT=<project_name>` to cmake to build only a specific module. Passing `no_tests` as the project name builds everything but the unit tests.
