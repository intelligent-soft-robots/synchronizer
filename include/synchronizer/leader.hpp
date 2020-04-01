#pragma once

#include "synchronizer/private/leader_sync.hpp"

namespace synchronizer
{
class Leader
{
public:
    /**
     * @brief Tools for setting a process managed by a Follower
     * to run either asynchronously to the process managed
     * by the leader (i.e. both processes run in parallel ignoring
     * each other) or to run synchronously to another process managed
     * the Leader (i.e. the two processes run alternatively)
     * At startup, the follower will init to async mode, and
     * will switch to sync mode when the corresponding call to
     * the start_sync of the leader is called.
     * Note that as follower needs to be started before its leader,
     * and can survive several leaders.
     * param[in] memory_segment segment id of the shared memory
     * shared between the leader and the follower
     */
    Leader(std::string memory_segment, bool sync_on_start = false);

    ~Leader();

    /** @brief Request the leader and the follower to
     * start running synchronously
     * param[in] the timeout (in micro seconds)
     *  the leader should wait to get the
     * acknowledgment from the follower the request to run sync has
     * been received. Infinite wait if negative.
     */
    void start_sync(long int timeout_us = -1);

    /** @brief Request the leader and follower to
     * switch to async mode
     */
    void stop_sync();

    /**
     * @brief Either does nothing (async mode)
     * or uses internally wait/notify functions to alternate run with
     * the follower (sync mode)
     */
    void pulse();

private:
    std::string memory_segment_;
    std::unique_ptr<Leader_sync> sync_;
};
}
