#pragma once

#include <memory>
#include <string>
#include "real_time_tools/spinner.hpp"
#include "shared_memory/locked_condition_variable.hpp"
#include "shared_memory/shared_memory.hpp"

namespace synchronizer
{
class Follower_sync

{
public:
    Follower_sync(std::string memory_segment_,
                  double async_frequency,
                  bool sync_on_start = false);
    ~Follower_sync();
    void reinit();
    void clean_memory();
    void wait();
    void notify();
    bool pulse();
    bool read_sync_mode();

private:
    std::unique_ptr<shared_memory::LockedConditionVariable> cv_;
    bool is_sync_mode_;
    bool previous_mode_;
    std::string memory_segment_;
    real_time_tools::Spinner spinner_;
    double async_frequency_;
    bool sync_on_start_;
};
}
