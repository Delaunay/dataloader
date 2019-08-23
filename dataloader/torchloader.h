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
    TorchLoader(Dataset const& dataset, const Sampler& sampler, std::size_t batch_size_, std::size_t worker_cout = 6, std::size_t buffering_=1, int seed=0, std::size_t io=0):
        loader(dataset, sampler, batch_size_, worker_cout, buffering_, seed, io)
    {}

    // return a NCHW ui8 tensor
    std::tuple<at::Tensor, at::Tensor> get_next_item(){
        std::tuple<std::vector<uint8_t>, std::vector<int>> batch = loader.get_next_item();

        return reduce_to_tensor(batch);
    }

    std::size_t const img_size = 3 * 224 * 224;

    std::tuple<torch::Tensor, torch::Tensor> reduce_to_tensor(const std::tuple<std::vector<uint8_t>, std::vector<int>> &future_batch);

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
