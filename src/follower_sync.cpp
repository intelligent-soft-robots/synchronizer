#include "synchronizer/private/follower_sync.hpp"

namespace synchronizer
{
Follower_sync::Follower_sync(std::string memory_segment,
                             double async_frequency,
                             bool sync_on_start)
    : memory_segment_(memory_segment),
      async_frequency_(async_frequency),
      sync_on_start_(sync_on_start)
{
    reinit();
}

Follower_sync::~Follower_sync()
{
    clean_memory();
}

void Follower_sync::clean_memory()
{
    shared_memory::LockedConditionVariable::clean(memory_segment_ +
                                                  std::string("_cond_var"));
    shared_memory::clear_shared_memory(memory_segment_);
}

// restarting fresh, i.e recreating the condition variable and the
// shared memory segment
void Follower_sync::reinit()
{
    clean_memory();
    cv_.reset(new shared_memory::LockedConditionVariable(
        memory_segment_ + std::string("_cond_var"), false));
    // will be used to get info from the leader that sync mode changed
    shared_memory::set(memory_segment_, "sync", sync_on_start_);
    // will be used to inform the leader request to change mode
    // has been received
    shared_memory::set(memory_segment_, "sync_shake", false);
    is_sync_mode_ = sync_on_start_;
    previous_mode_ = sync_on_start_;
    // for running at desired frequency when in async mode
    spinner_.initialize();
    spinner_.set_frequency(async_frequency_);
    // for locking the scope
    if (sync_on_start_)
    {
        cv_->lock_scope();
        wait();
    }
}

void Follower_sync::wait()
{
    cv_->wait();
}

void Follower_sync::notify()
{
    cv_->notify_one();
}

// returning true if this instance should be destroyed
// and replaced by a new one
bool Follower_sync::pulse()
{
    // reading from shared memory which mode
    // we are in
    bool reset = read_sync_mode();
    if (reset)
    {
        // reset is true when switching from
        // sync to async. This instance should then
        // be destroyed and replaced by a new one
        return true;
    }
    if (is_sync_mode_)
    {
        // in sync mode, we use the condition variable
        notify();
        wait();
    }
    else
    {
        // in async mode, we run at desired frequency
        spinner_.spin();
    }
    return false;
}

// returns true if reset is requested, i.e.
// sync mode just been turned off
bool Follower_sync::read_sync_mode()
{
    // reading from the shared memory what mode the leader
    // requests
    shared_memory::get(memory_segment_, "sync", is_sync_mode_);
    if (is_sync_mode_ && !previous_mode_)
    {
        // switching from async to sync
        // telling the leader we go the request
        shared_memory::set(memory_segment_, "sync_shake", true);
        // init the condition variable
        cv_->lock_scope();
        wait();
    }
    if (previous_mode_ && !is_sync_mode_)
    {
        // switching from sync to async.
        // this is solved by returning true,
        // i.e info this instance should be destroyed
        // and replaced by a new one (which will be async)
        previous_mode_ = is_sync_mode_;
        return true;
    }
    // to monitor mode switch
    previous_mode_ = is_sync_mode_;
    return false;
}
}
