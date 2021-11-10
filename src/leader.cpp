#include "synchronizer/leader.hpp"

namespace synchronizer
{
Leader::Leader(std::string memory_segment, bool sync_on_start)
    : memory_segment_(memory_segment)
{
    sync_.reset(new Leader_sync(memory_segment, sync_on_start));

    // making sure two leaders do not coexist on the same memory segment
    if (shared_memory::segment_exists(memory_segment + "_leader"))
    {
        throw std::runtime_error(
            std::string(
                "A synchronizer::leader is contructed under memory segment ") +
            memory_segment +
            std::string(", which has already another leader active"));
    }
    else
    {
        shared_memory::set<bool>(memory_segment + "_leader", "leader", true);
    }
}

Leader::~Leader()
{
    if (sync_ != nullptr)
    {
        try
        {
            stop_sync();
        }
        catch (...)
        {
        }
    }
    shared_memory::clear_shared_memory(memory_segment_ + "_leader");
    shared_memory::delete_segment(memory_segment_ + "_leader");
}

void Leader::start_sync(long int timeout_us)
{
    if (sync_ == nullptr)
    {
        sync_.reset(new Leader_sync(memory_segment_, true));
    }

    sync_->start_sync(timeout_us);
}

void Leader::stop_sync()
{
    sync_->stop_sync();
    sync_.reset(nullptr);
}

void Leader::pulse()
{
    sync_->pulse();
}
}  // namespace synchronizer
