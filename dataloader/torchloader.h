#ifndef DATALOADER_TORCHLOADER_HEADER_H
#define DATALOADER_TORCHLOADER_HEADER_H

#include <torch/extension.h>

#include "dataloader.h"
#include "utils.h"

#undef DLOG
#define DLOG(...)

// Used for the C++ JIT side
class TorchLoader{
public:
    TorchLoader(ImageFolder const& dataset, std::size_t batch_size_, std::size_t worker_cout = 6, std::size_t buffering_=1, int seed=0, std::size_t io=0):
        loader(dataset, batch_size_, worker_cout, buffering_, seed, io)
    {}

    // return a NCHW ui8 tensor
    at::Tensor get_next_item(){
        auto batch = loader.get_next_item();
        return reduce_to_tensor(batch);
    }

    std::size_t const img_size = 3 * 224 * 224;

    torch::Tensor reduce_to_tensor(std::vector<uint8_t> const& future_batch);

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
