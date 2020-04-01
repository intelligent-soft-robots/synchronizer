#include "synchronizer/private/leader_sync.hpp"

namespace synchronizer
{
Leader_sync::Leader_sync(std::string memory_segment, bool sync_on_start)
    : memory_segment_(memory_segment),
      is_sync_mode_(sync_on_start),
      cv_(memory_segment + std::string("_cond_var"))
{
    // Follower would have initialized this shared memory object
    // when starting
    try
    {
        bool foo;
        shared_memory::get(memory_segment, "sync", foo);
    }
    catch (...)
    {
        throw std::runtime_error(
            std::string(
                "synchronizer::Leader should be started after an instance") +
            std::string(" of synchronizer::Follower of the same memory segment "
                        "has been started"));
    }

    if (sync_on_start)
    {
        cv_.lock_scope();
        notify();
        wait();
    }
}

void Leader_sync::wait()
{
    cv_.wait();
}

void Leader_sync::notify()
{
    cv_.notify_one();
}

void Leader_sync::pulse()
{
    if (!is_sync_mode_)
    {
        return;
    }
    notify();
    wait();
}

void Leader_sync::start_sync(long int timeout_us)
{
    // for tracking internally
    is_sync_mode_ = true;
    // writing in shared memory request to Follower that
    // we want to switch to sync mode
    shared_memory::set(memory_segment_, "sync", true);
    bool shaked = false;
    long int timeout = 0;
    // waiting for the Follower to acknowledge
    while (!shaked)
    {
        shared_memory::get(memory_segment_, "sync_shake", shaked);
        usleep(50);
        if (timeout_us > 0)
        {
            timeout += 50;
            if (timeout > timeout_us)
            {
                throw std::runtime_error(
                    "failed to get hand shake from synchronized Follower");
            }
        }
    }
    // initializing the condition variable
    cv_.lock_scope();
    notify();
    wait();
}

void Leader_sync::stop_sync()
{
    // it is illegal to switch to async when
    // already in async
    if (!is_sync_mode_)
    {
        throw std::runtime_error(
            std::string("synchronizer::Leader stopping synchronous") +
            std::string(" mode while already being in synchronous mode"));
    }
    // informing the Follower we are changing mode
    shared_memory::set(memory_segment_, "sync", false);
    // the Follower will reset the shared memory. So letting
    // the current segment go
    shared_memory::delete_segment(memory_segment_);
    // giving hand back to Follower
    notify();
}
}
