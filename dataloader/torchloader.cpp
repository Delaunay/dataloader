#include "torchloader.h"


torch::Tensor TorchLoader::reduce_to_tensor(const std::vector<uint8_t> &future_batch){
    auto options =
      torch::TensorOptions()
        .dtype(torch::kUInt8)
        .device(torch::kCPU)
        .requires_grad(false);

    DLOG("Allocating batch");
    at::Tensor batch = torch::zeros({(long int)(loader.batch_size), 3, 224, 224}, options);

    DLOG("Copying result to tensor");
    TimeIt reduce_time;
    //for(std::size_t i = 0; i < loader.batch_size; ++i){
    //    Image const& img = future_batch[i];
    //    memcpy(batch.data<unsigned char>() + i * img_size, img.data(), img_size);
    //}

    memcpy(batch.data<uint8_t>(), future_batch.data(), img_size * loader.batch_size);

    RuntimeStats::stat().insert_reduce(reduce_time.stop(), loader.batch_size);

    DLOG("Tensor Ready");
    return batch;
}
