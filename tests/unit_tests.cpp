#include "gtest/gtest.h"
#include "real_time_tools/realtime_check.hpp"
#include "real_time_tools/spinner.hpp"
#include "real_time_tools/thread.hpp"
#include "synchronizer/follower.hpp"
#include "synchronizer/leader.hpp"

using namespace synchronizer;

class Synchronizer_tests : public ::testing::Test
{
protected:
    void SetUp()
    {
    }
    void TearDown()
    {
    }
};

TEST_F(Synchronizer_tests, frequency_aync)
{
    Follower s("ut_sync", 250);

    real_time_tools::RealTimeCheck rc(100, 200);

    for (int i = 0; i < 100; i++)
    {
        s.pulse();
        rc.tick();
    }

    int t, st;
    double tf, sw, cf;
    double average_frequency, wf;

    rc.get_statistics(t, st, tf, sw, average_frequency, cf, wf);

    ASSERT_LT(average_frequency, 270);
    ASSERT_GT(average_frequency, 230);
}

void* _run_follower(void* freq)
{
    shared_memory::clear_shared_memory("ut_sync_exchange");

    double frequency = *(static_cast<double*>(freq));

    shared_memory::set("ut_sync_exchange", "running", true);

    bool running = true;

    Follower s("ut_sync", frequency);

    real_time_tools::RealTimeCheck rc(frequency, 2 * frequency);

    while (running)
    {
        s.pulse();
        rc.tick();
        shared_memory::get("ut_sync_exchange", "running", running);
    }

    int t, st;
    double tf, sw, cf;
    double average_frequency, wf;

    rc.get_statistics(t, st, tf, sw, average_frequency, cf, wf);

    shared_memory::set("ut_sync_exchange", "frequency", average_frequency);

    return nullptr;
}

TEST_F(Synchronizer_tests, test_sync)
{
    // creating a follower thread with async frequency
    // of 100

    real_time_tools::RealTimeThread thread;
    double frequency = 100.0;
    thread.create_realtime_thread(_run_follower, &frequency);
    usleep(1000);

    // creating a Leader requesting a sync
    // frequency of 300

    real_time_tools::Spinner spinner;
    spinner.set_frequency(300);
    Leader Leader("ut_sync");
    Leader.start_sync();
    for (int iteration = 0; iteration < 100; iteration++)
    {
        Leader.pulse();
        spinner.spin();
    }

    // requesting follower to stop

    Leader.stop_sync();
    shared_memory::set("ut_sync_exchange", "running", false);
    thread.join();

    // getting frequency the follower was running at, should be around 300

    double follower_frequency;
    shared_memory::get("ut_sync_exchange", "frequency", follower_frequency);
    ASSERT_GT(follower_frequency, 250);
    ASSERT_LT(follower_frequency, 350);
}

TEST_F(Synchronizer_tests, test_throw_stop_before_start)
{
    real_time_tools::RealTimeThread thread;
    double frequency = 100.0;
    thread.create_realtime_thread(_run_follower, &frequency);
    usleep(2000);

    real_time_tools::Spinner spinner;
    spinner.set_frequency(300);
    Leader leader("ut_sync");

    ASSERT_THROW(leader.stop_sync(), std::runtime_error);

    shared_memory::set("ut_sync_exchange", "running", false);
    thread.join();
}

TEST_F(Synchronizer_tests, test_throw_if_no_follower)
{
    ASSERT_THROW(Leader leader("ut_sync"), std::runtime_error);
}

TEST_F(Synchronizer_tests, test_no_two_Leaders)
{
    Follower follower("ut_sync", 0);
    Leader leader("ut_sync");
    ASSERT_THROW(Leader leader2("ut_sync"), std::runtime_error);
}

TEST_F(Synchronizer_tests, test_no_two_followers)
{
    Follower follower("ut_sync", 0);
    ASSERT_THROW(Follower follower2("ut_sync", 0), std::runtime_error);
}
