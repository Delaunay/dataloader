#include "dataset.h"
#include "torchloader.h"
#include "jpeg.h"
#include "utils.h"
#include "runtime.h"
#include "pool.h"
#include "io.h"
#include "loader.h"

#include <torch/all.h>
#include <torch/script.h>

#include <memory>
#include <ctime>
#include <sstream>
#include <cstdio>
#include <cstring>

#include "utils.h"

int main(int argc, const char* argv[]){
    int image_to_load = 32;
    int seed = int(time(nullptr));
    std::size_t thread_count = 16;
    std::size_t batch_size = 32;
    std::size_t buffering = 3;
    std::size_t max_io_thread = 4;
    std::string model_file = "/home/user1/local_mila/dataloader/build/resnet18.pt";

    const char* data_loc = "/home/user1/test_database/imgnet/ImageNet2012_jpeg/train/";
    //const char* data_loc = "/media/setepenre/UserData/tmp/fake";

    for(int i = 0; i < argc; ++i){
        std::string arg = std::string(argv[i]);
        if ("--data" == arg) {
            data_loc = argv[i + 1];
        }
        if ("-n" == arg) {
            std::stringstream ss(argv[i + 1]);
            ss >> image_to_load;
        }
        if ("--seed" == arg){
            std::stringstream ss(argv[i + 1]);
            ss >> seed;
        }
        if ("-j" == arg){
            std::stringstream ss(argv[i + 1]);
            ss >> thread_count;
        }
        if ("-b" == arg){
            std::stringstream ss(argv[i + 1]);
            ss >> batch_size;
        }
        if ("-p" == arg){
            std::stringstream ss(argv[i + 1]);
            ss >> buffering;
        }
        if ("-io" == arg){
            std::stringstream ss(argv[i + 1]);
            ss >> max_io_thread;
        }
    }

    make_io_lock(max_io_thread);

    DLOG("Reading Pytorch model to jit");
    torch::jit::script::Module module = torch::jit::load(model_file);

    //try{
        DLOG("Init data set");
        Dataset dataset("ImageFolder", data_loc, true);

        Sampler sampler("RandomSampler", dataset.size(), seed);

        TorchLoader dataloader(dataset, sampler, batch_size, thread_count, buffering, seed);

        DLOG("Starting training");
        TimeIt loop_time;
        for(int i = 0; i < image_to_load; ++i){
            std::vector<torch::jit::IValue> inputs;
            std::tuple<torch::Tensor, torch::Tensor> in = dataloader.get_next_item();

            inputs.push_back(std::get<0>(in));
            inputs.push_back(std::get<1>(in));

            at::Tensor out = module.forward(inputs).toTensor();

            std::cout << out.slice(/*dim=*/1, /*start=*/0, /*end=*/5) << '\n';
        }

        double loop = loop_time.stop();
        RuntimeStats::stat().report(loop, thread_count);

        dataloader.report();
    /*
    } catch (const FS_NAMESPACE::filesystem_error& e){
        printf("%s\n", e.what());
        return -1;
    }*/


    return 0;
}
