# Code Character Simulator

## Setup Instructions

1. `git clone --recursive https://gitlab.com/codecharacter/simulator.git`

2. Install CMake (> v3.6.2)

3. `mkdir build && cd build`

4. `export LD_LIBRARY_PATH=<your_install_location>/lib`

5. `cmake .. --DCMAKE_INSTALL_PREFIX=<your_install_location>`

6. `make`

7. `make install`

8. To run the simulator, `<your_install_location/bin/simulator`

To run the unit tests, `<your_install_location>/bin/tester`
