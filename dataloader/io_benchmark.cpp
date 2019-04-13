#include "dataset.h"
#include "dataloader.h"
#include "jpeg.h"
#include "utils.h"
#include "runtime.h"
#include "pool.h"
#include "io.h"

#include "loader.h"

#include <ctime>
#include <sstream>
#include <cstdio>
#include <cstring>

int main(int argc, const char* argv[]){

    int image_to_load = 32;
    int seed = int(time(nullptr));
    std::size_t thread_count = 16;
    std::size_t batch_size = 32;
    std::size_t buffering = 3;
    std::size_t max_io_thread = 4;

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
    }

    make_io_lock(max_io_thread);

    #ifdef USE_CPP
    try{
    #endif
        ImageFolder dataset(data_loc, single_threaded_loader);
        DataLoader dataloader(dataset, batch_size, thread_count, buffering, seed);

        TimeIt loop_time;
        for(int i = 0; i < image_to_load; ++i){
            dataloader.get_next_item();
        }

        double loop = loop_time.stop();
        RuntimeStats::stat().report(loop, thread_count, max_io_thread);

        dataloader.report();

    #ifdef USE_CPP
    } catch (const FS_NAMESPACE::filesystem_error& e){
        printf("%s\n", e.what());
        return -1;
    }
    #endif

    return 0;
}
