import sys
import struct

SECONDS_PER_TICK = 0.01

file = open(sys.argv[1], "rb")
data = file.read()
(magic, completed_levels, unlocked_abilities) = struct.unpack("III", data[0:12])
completion_times = struct.unpack("128I", data[12:12+128*4])
(speedrun_ticks) = struct.unpack("I", data[12+128*4:12+128*4 + 4])
speedrun_time = float(speedrun_ticks[0]) * SECONDS_PER_TICK

print(f"Completed levels: {completed_levels}")
print(f"Unlocked abilities: {bin(unlocked_abilities)}")
print(f"Speedrun time: {speedrun_time} sec")
print("Completion times:")

shortest_level_time = 100000000.0
shortest_level_index = 0.0
longest_level_time = 0.0
longest_level_index = 0.0

for i in range(0, 128):
    time = completion_times[i] * SECONDS_PER_TICK
    if time > 0:
        print(f"Level #{i+1}: {time} sec")
        if time < shortest_level_time:
            shortest_level_index = i
            shortest_level_time = time
        if time > longest_level_time:
            longest_level_index = i
            longest_level_time = time

print(f"Shortest level: #{shortest_level_index+1} ({shortest_level_time} sec)")
print(f"Longest level: #{longest_level_index+1} ({longest_level_time} sec)")

file.close()
