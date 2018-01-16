cd /root/codecharacter/build

# Make player libraries
cmake .. -DBUILD_PROJECT=player_code
make -j $(expr $(grep -c '^processor' /proc/cpuinfo) - 1) install

# Copy output libraries
cp /root/simulator/lib/libplayer_1_code.so /root/output_libs
cp /root/simulator/lib/libplayer_2_code.so /root/output_libs
