#ifndef DATALOADER_PYDATALOADER_HEADER_H
#define DATALOADER_PYDATALOADER_HEADER_H

#include "dataloader.h"

#include <torch/all.h>
//#include <torch/csrc/utils/tensor_numpy.h>
//#include <pybind11/pybind11.h>


#include <torch/csrc/utils/tensor_numpy.h>

class PyLoader{
public:
#define PYTENSOR PyObject*

    PyLoader(ImageFolder const& dataset, std::size_t batch_size_, std::size_t worker_cout = 6, std::size_t buffering_=1, int seed=0, std::size_t io=0):
        loader(dataset, batch_size_, worker_cout, buffering_, seed, io)
    {}

    // return a NCHW ui8 tensor
    PYTENSOR get_next_item(){
        loader.send_next_batch();

        return to_tensor_python(reduce_to_tensor(loader.get_future_batch()));
    }

    PYTENSOR to_tensor_python(at::Tensor const& tensor){
        return torch::utils::tensor_to_numpy(tensor);
    }


    std::size_t const img_size = 3 * 224 * 224;

    torch::Tensor reduce_to_tensor(std::vector<std::shared_future<Image>>& future_batch);

    void report() const {
        loader.report();
    }

    void shutdown(){
        loader.shutdown();
    }

private:
    DataLoader loader;
};

#endif
