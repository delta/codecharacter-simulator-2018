cd /root/codecharacter/build

# Copy input libraries
cp /root/input_libs/* /root/simulator/lib/

# Run simulator
cd /root/simulator/bin
./main

# Copy game log to output dir
mkdir /root/outpur_log
cp /root/simulator/bin/game.log /root/output_log
