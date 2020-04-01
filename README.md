# Synchronizer

## Overview

Allows a leader process to synchronize with a follower process.
The mode (synchronous or asynchronous) can be changed programmatically during runtime.

For exemple, in the follower process:

```cpp

// running at 200Hz when in async mode
double frequency = 200;

// will have loop running sync or async
synchronizer::Follower synchronizer("synchronizer_id",frequency);

while (running)
{
   // code doing things

   // pulsing instead of sleeping
   synchronizer.pulse();
}

```
and in the leader process:

```cpp

synchronizer::Leader leader ("synchronizer_id");

leader.start_sync();
for(int i=0;i<1000;i++)
{
    // code doing thing

    // sleeping to get desired frequency
   usleep(1000); 

    // requesting the follower to run one iteration
    leader.pulse()

}

leader.stop_sync();

```

The expected behavior:

- when follower is started, it runs at 200Hz
- when leader is started, leader and follower run at same frequency (~ 1Khz)
- when leader exit (calls stop_sync), follower returns at 200Hz

## Python

Python bindings are generated via Pybind11. 
Python/c++ follower/leader processes  are compatible one with another (e.g. follower in python, leader in cpp)

## Demos

For concrete examples (c++ and python), look at the demos folder. Note that the follower must be started before leader.

## Limitation

A follower can be linked to only one leader (at a time), and a leader to one follower.

## Installation and compilation

This is a catkin package. 
Recommanded method for cloning is to use [treep](https://pypi.org/project/treep/) with configuration [treep_machines_in_motion](https://github.com/machines-in-motion/treep_machines_in_motion) and project "CORE_ROBOTICS"

## Licence

BSD-3 clause

## Copyright

Max Planck Gesellschaft,  University of New York

## Author

Vincent Berenz, Max Planck Institute for Intelligent Systems


