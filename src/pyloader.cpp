#include <pybind11/pybind11.h>

#include "torchloader.h"

namespace py = pybind11;

PYBIND11_MODULE(cpploader, m) {
    py::class_<ImageFolder>(m, "ImageFolder")
            .def(py::init<const std::string&, bool>());

    // PyLoader(ImageFolder const& dataset, std::size_t batch_size_, std::size_t worker_cout = 6, std::size_t buffering_=1, int seed=0
    py::class_<PyLoader>(m, "Loader")
        .def(py::init<ImageFolder const&, std::size_t, std::size_t, std::size_t, int, int>())
        .def("next", &PyLoader::get_next_item)
        .def("shutdown", &PyLoader::shutdown)
        .def("report", &PyLoader::report);
}

/*
static auto registry =
  torch::jit::("my_ops::warp_perspective", &warp_perspective)
  .op("loader::another_op", &another_op)
  .op("my_ops::and_another_op", &and_another_op);*/


/*
cd lib
mv libcpploader.so cpploader.so
python -c "import cpploader; img = cpploader.ImageFolder('/media/setepenre/UserData/tmp/fake', True); data = cpploader.Loader(img, 256, 6, 2, 0, 0); data.shutdown(); data.report(); print('done')"


import cpploader

img = cpploader.ImageFolder('/media/setepenre/UserData/tmp/fake', True)
data = cpploader.Loader(img, 256, 6, 8, 10, 0)

for i in range(1, 10):
    batch = data.next()

    print(data)

    if i > 10:
        break


data.shutdown()
print('done')

 */
