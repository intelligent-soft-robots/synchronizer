#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include "shared_memory/locked_condition_variable.hpp"
#include "shared_memory/shared_memory.hpp"

namespace synchronizer
{
class Leader_sync
{
public:
    Leader_sync(std::string memory_segment, bool sync_on_start = false);
    void wait();
    void notify();
    void pulse();
    void start_sync(long int timeout);
    void stop_sync();

private:
    shared_memory::LockedConditionVariable cv_;
    std::string memory_segment_;
    bool is_sync_mode_;
};
}
