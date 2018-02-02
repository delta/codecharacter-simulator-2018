cd /root/codecharacter/build

# Copy input libraries
cp /root/input_libs/* /root/simulator/lib/

# Run simulator
cd /root/simulator/bin
./main

# Copy game log to output dir
if [ -f /root/simulator/bin/game.log  ]; then
	cp /root/simulator/bin/game.log /root/output_log
fi

# Copy debug logs to output dir
if [ -f /root/simulator/bin/player_1.dlog  ]; then
	cp /root/simulator/bin/player_1.dlog /root/output_log
fi

if [ -f /root/simulator/bin/player_2.dlog  ]; then
	cp /root/simulator/bin/player_2.dlog /root/output_log
fi
