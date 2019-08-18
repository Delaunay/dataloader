#include "torchloader.h"


std::tuple<torch::Tensor, torch::Tensor> TorchLoader::reduce_to_tensor(const std::tuple<std::vector<uint8_t>, std::vector<int>> &future_batch){
    auto options =
      torch::TensorOptions()
        .dtype(torch::kUInt8)
        .device(torch::kCPU)
        .requires_grad(false);

    DLOG("Allocating batch");
    at::Tensor batch = torch::zeros({(long int)(loader.batch_size), 3, 224, 224}, options);

    auto labels_opt =
      torch::TensorOptions()
        .dtype(torch::kLong)
        .device(torch::kCPU)
        .requires_grad(false);
    at::Tensor labels = torch::zeros({(long int)(loader.batch_size)}, labels_opt);

    DLOG("Copying result to tensor");
    TimeIt reduce_time;
    //for(std::size_t i = 0; i < loader.batch_size; ++i){
    //    Image const& img = future_batch[i];
    //    memcpy(batch.data<unsigned char>() + i * img_size, img.data(), img_size);
    //}

    memcpy(batch.data<uint8_t>(), std::get<0>(future_batch).data(), img_size * loader.batch_size);
    memcpy(labels.data<int64_t>(), std::get<1>(future_batch).data(), loader.batch_size);

    RuntimeStats::stat().insert_reduce(reduce_time.stop(), loader.batch_size);

    DLOG("Tensor Ready");
    return std::make_tuple(batch, labels);
}
