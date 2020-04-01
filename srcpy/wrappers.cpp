#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include "synchronizer/follower.hpp"
#include "synchronizer/leader.hpp"

using namespace synchronizer;

PYBIND11_MODULE(synchronizer, m)
{
    pybind11::class_<Follower>(m, "Follower")
        .def(pybind11::init<std::string, double>())
        .def("pulse", &Follower::pulse);

    pybind11::class_<Leader>(m, "Leader")
        .def(pybind11::init<std::string>())
        .def("pulse", &Leader::pulse)
        .def(
            "start_sync", &Leader::start_sync, pybind11::arg("timeout_us") = -1)
        .def("stop_sync", &Leader::stop_sync);
}
