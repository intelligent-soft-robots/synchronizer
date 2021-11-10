#pragma once

#include "synchronizer/private/follower_sync.hpp"

namespace synchronizer
{
class Follower
{
public:
    /**
     * @brief Tools for setting a process to run alternatively
     * asynchronously (i.e. at own desired frequency) or
     * synchronously to another process managed by a Leader
     * using the same memory_segment id.
     * At startup, the follower will init to async mode, and
     * will switch to synchronous if the function set_sync
     * of the leader is called.
     * Note that as follower needs to be started before its leader,
     * and can survive several leaders.
     * param[in] memory_segment segment id of the shared memory
     * shared between the leader and the follower
     * param[in] frequency the follower will run at when in async mode
     */
    Follower(std::string memory_segment_,
             double async_frequency,
             bool sync_on_start = false);

    ~Follower();

    /**
     * @brief Either sleep to matches the frequency (async mode)
     * or uses internally wait/notify functions to alternate run with
     * the leader (sync mode)
     */
    void pulse();

private:
    std::string memory_segment_;
    Follower_sync sync_;
};
}  // namespace synchronizer
