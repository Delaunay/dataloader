#include <pybind11/pybind11.h>
#include <torch/extension.h>

#include "pytorchloader.h"
#include "dataset.h"

namespace py = pybind11;

PYBIND11_MODULE(TORCH_EXTENSION_NAME, m) {
    py::class_<Dataset>(m, "Dataset")
            .def(py::init<const std::string&, const std::string&, bool>())
            .def("size", &Dataset::size);

    py::class_<Sampler>(m, "Sampler")
            .def(py::init<const std::string&, int, int>());

    // PyLoader(ImageFolder const& dataset, std::size_t batch_size_, std::size_t worker_cout = 6, std::size_t buffering_=1, int seed=0
    py::class_<PyTorchLoader>(m, "Loader")
        .def(py::init<Dataset const&, Sampler const&, std::size_t, std::size_t, std::size_t, int, int>())
        .def("next", &PyTorchLoader::get_next_item)
        .def("shutdown", &PyTorchLoader::shutdown)
        .def("report", &PyTorchLoader::report);
}

