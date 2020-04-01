#include "synchronizer/follower.hpp"

namespace synchronizer
{
Follower::Follower(std::string memory_segment,
                   double async_frequency,
                   bool sync_on_start)
    : memory_segment_(memory_segment),
      sync_(memory_segment, async_frequency, sync_on_start)
{
    // making sure two followers do not coexist on the same memory segment
    if (shared_memory::segment_exists(memory_segment + "_follower"))
    {
        throw std::runtime_error(
            std::string("A synchronizer::follower is contructed under memory "
                        "segment ") +
            memory_segment +
            std::string(", which has already another follower active"));
    }
    else
    {
        shared_memory::set<bool>(
            memory_segment + "_follower", "follower", true);
    }
}

Follower::~Follower()
{
    shared_memory::clear_shared_memory(memory_segment_ + "_follower");
    shared_memory::delete_segment(memory_segment_ + "_follower");
}

void Follower::pulse()
{
    bool reset = sync_.pulse();
    if (reset)
    {
        sync_.reinit();
        sync_.pulse();
    }
}
}
