cd /root/codecharacter/build

# Copy input libraries
cp /root/input_libs/* /root/simulator/lib/

# Run simulator
cd /root/simulator/bin
./main

# Copy game log to output dir
cp /root/simulator/bin/game.log /root/output_log

# Copy debug logs to output dir
cp /root/simulator/bin/player_1.dlog /root/output_log
cp /root/simulator/bin/player_2.dlog /root/output_log
