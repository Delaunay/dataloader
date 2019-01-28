#include "torchloader.h"
#include <torch/torch.h>


torch::Tensor TorchLoader::reduce_to_tensor(std::vector<std::shared_future<Image>>& future_batch){
    auto options =
      torch::TensorOptions()
        .dtype(torch::kUInt8)
        .device(torch::kCPU)
        .requires_grad(false);

    DLOG("Allocating batch");
    at::Tensor batch = at::zeros({(long int)(loader.batch_size), 3, 224, 224}, options);

    DLOG("Copying result to tensor");
    TimeIt reduce_time;
    for(std::size_t i = 0; i < loader.batch_size; ++i){
        std::shared_future<Image>& fut = future_batch[i];
        Image img = fut.get();

        memcpy(batch.data<unsigned char>() + i * img_size, img.data(), img_size);
    }
    RuntimeStats::stat().insert_reduce(reduce_time.stop(), loader.batch_size);

    DLOG("Tensor Ready");
    return batch;
}
