import synchronizer, time, shared_memory_py

leader = synchronizer.Leader("sync_mem")


# running in sync mode for 4 seconds.
# as fast as possible
print("sync: follower running as fast as possible")
leader.start_sync(-1)
time_start = time.time()
while time.time() - time_start < 4:
    leader.pulse()

# stopping synchronization,
# follower can run at whatever frequency
# it is set to (in this case 3Hz)
leader.stop_sync()
print("async: follower running at dedicated frequency")
time.sleep(3)

# running in sync mode for 4 seconds.
# as fast as possible
print("sync: follower running as fast as possible")
leader.start_sync(-1)
time_start = time.time()
while time.time() - time_start < 4:
    leader.pulse()

print("leader exit")
