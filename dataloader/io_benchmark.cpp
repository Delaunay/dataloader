#include "dataset.h"
#include "dataloader.h"
#include "jpeg.h"
#include "utils.h"
#include "runtime.h"
#include "pool.h"
#include "io.h"
#include "loader.h"

#include <ctime>
#include <cstdio>
#include <cstring>

#include <iostream>
#include <sstream>

int main(int argc, const char* argv[]){

    int image_to_load = 32;
    int seed = int(time(nullptr));
    std::size_t thread_count = 16;
    std::size_t batch_size = 32;
    std::size_t buffering = 3;
    std::size_t max_io_thread = 4;
    bool zip_dataset = false;
    bool seq_sampler = false;

    //const char* data_loc = "/home/user1/test_database/imgnet/ImageNet2012_jpeg/train/";
    const char* data_loc = "/media/setepenre/UserData/tmp/fake";

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
        if ("-zip" == arg){
            std::stringstream ss(argv[i + 1]);
            ss >> zip_dataset;
        }
        if ("-seq" == arg){
            std::stringstream ss(argv[i + 1]);
            ss >> seq_sampler;
        }
    }

    printf("io: %ul\n", max_io_thread);
    make_io_lock(max_io_thread);

    std::string dataset_bck = "ImageFolder";
    std::string sampler_bck = "RandomSampler";

    if (zip_dataset){
        dataset_bck = "ZippedImageFolder";
    }
    if (seq_sampler){
        sampler_bck = "SequentialSampler";
    }
    printf("sampler: %s\n", sampler_bck.c_str());
    printf("dataset: %s\n", dataset_bck.c_str());

    Dataset dataset(dataset_bck, data_loc);
    Sampler sampler(sampler_bck, dataset.size(), seed);
    DataLoader dataloader(
        dataset,
        sampler,
        batch_size,
        single_threaded_loader,
        thread_count,
        buffering,
        seed,
        max_io_thread
    );

    TimeIt loop_time;
    for(int i = 0; i < image_to_load; ++i){
        auto batch = dataloader.get_next_item();
        std::cout << i << std::endl;
    }

    double loop = loop_time.stop();
    dataloader.report();

    return 0;
}
