#ifndef DATALOADER_PYTORCHLOADER_HEADER_H
#define DATALOADER_PYTORCHLOADER_HEADER_H

#include "torchloader.h"

#include <torch/csrc/utils/tensor_numpy.h>


// Used on the python side
class PyTorchLoader{
public:
    PyTorchLoader(ImageFolder const& dataset, std::size_t batch_size_, std::size_t worker_cout = 6, std::size_t buffering_=1, int seed=0, std::size_t io=0):
        loader(dataset, batch_size_, worker_cout, buffering_, seed, io)
    {}

    // return a NCHW ui8 tensor
    std::tuple<at::Tensor, at::Tensor> get_next_item(){
        return loader.get_next_item();
    }

    PyObject* to_numpy(at::Tensor const& tensor){
        return torch::utils::tensor_to_numpy(tensor);
    }

    void report() const {
        loader.report();
    }

    void shutdown(){
        loader.shutdown();
    }

private:
    TorchLoader loader;
};

#endif
