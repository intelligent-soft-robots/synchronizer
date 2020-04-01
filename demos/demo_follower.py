import synchronizer

follower = synchronizer.Follower("sync_mem",3)

counter = 0

while (True):
    counter+=1
    follower.pulse()
    print("pulse",counter)


    
