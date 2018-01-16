# Code Character Simulator

## Setup Instructions

1. Download [Boost 1.64](https://dl.bintray.com/boostorg/release/1.64.0/source/)

2. `git clone --recursive https://github.com/delta/codecharacter-simulator.git`

3. Install [CMake](https://cmake.org/download/) (> v3.9.6)

4. Install [Protobuf](https://github.com/google/protobuf/releases)

5. `mkdir build && cd build`

6. `export LD_LIBRARY_PATH=<your_install_location>/lib`

7. `cmake .. -DCMAKE_INSTALL_PREFIX=<your_install_location> -DBOOST_ROOT=<boost_download_location>`

8. `make`

9. `make install`

10. To run the simulator, `<your_install_location>/bin/simulator`

To run the unit tests, `<your_install_location>/bin/test`

Pass `-DBUILD_PROJECT=<project_name>` to cmake to build only a specific module. Passing `no_tests` as the project name builds everything but the unit tests.


## Docker image instructions

The compiler image takes player code and compiles it into player1 and player2 libraries.

The runner image takes a player1 library and a player2 library, simulates a game, and dumps the game.log.

To setup these images -

1. Install Docker (preferably enable non-root usage)

2. Run `./docker_build.sh` to build the images

3. `docker run -v <input_source_dir>:/root/codecharacter/src/player_code/src -v <output_libs_dir>:/root/output_libs -i deltanitt/codecharacter-runner` to compiple, where `<input_source_dir>` contains the file `player_code.cpp`.

4. `docker run -v <input_libs_dir>:/root/input_libs -v <output_log_dir>:/root/output_log -i deltanitt/codecharacter-runner` to run a game, where `<input_libs_dir>` contains the files `libplayer_1_code.so` and `libplayer_2_code.so`.
