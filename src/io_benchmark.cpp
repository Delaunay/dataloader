#include "dataset.h"
#include "dataloader.h"
#include "jpeg.h"
#include "utils.h"
#include "runtime.h"
#include "pool.h"
#include "io.h"

#include <ctime>
#include <sstream>
#include <cstdio>
#include <cstring>


using Path = ImageFolder::Path;


Image single_threaded_loader(std::tuple<Path, int, std::size_t> const& item){
    Path path; int label; std::size_t size;
    std::tie(path, label, size) = item;

    // Read
    TimeIt io_block_time;
    start_io();
    RuntimeStats::stat().insert_io_block(io_block_time.stop());

    TimeIt read_time;
    auto jpeg = JpegImage(path.c_str(), size);
    end_io();
    RuntimeStats::stat().insert_read(read_time.stop(), size);

    Transform trans;
    trans.hflip();

    // Transform
    TimeIt transform_time;
    jpeg.inplace_transform(trans);
    RuntimeStats::stat().insert_transform(transform_time.stop(), jpeg.size());

    // decode
    TimeIt decode_time;
    Image img = jpeg.decode();
    RuntimeStats::stat().insert_decode(decode_time.stop(), img.size());

    // Scale
    TimeIt scale_time;
    img.inplace_scale(224, 224);
    RuntimeStats::stat().insert_scaling(scale_time.stop(), img.size());

    RuntimeStats::stat().increment_count();

    return img;
}


int main(int argc, const char* argv[]){

    int image_to_load = 32;
    int seed = int(time(nullptr));
    std::size_t thread_count = 16;
    std::size_t batch_size = 32;
    std::size_t buffering = 3;
    std::size_t max_io_thread = 4;

    make_io_lock(max_io_thread);

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
        if ("-j" ==arg){
            std::stringstream ss(argv[i + 1]);
            ss >> thread_count;
        }
        if ("-b" ==arg){
            std::stringstream ss(argv[i + 1]);
            ss >> batch_size;
        }
        if ("-p" ==arg){
            std::stringstream ss(argv[i + 1]);
            ss >> buffering;
        }
    }

    Transform trans;
    trans.hflip();

    try{
        ImageFolder dataset(data_loc, single_threaded_loader);
        // ImageFolder const& dataset, int batch_size, int worker_cout = 6, int buffering=1, int seed=0
        DataLoader dataloader(dataset, batch_size, thread_count, buffering, seed);

        //std::vector<std::shared_future<Image>> images;

        TimeIt loop_time;
        for(int i = 0; i < image_to_load; ++i){
            dataloader.get_next_item();
        }


        double loop = loop_time.stop();
        RuntimeStats::stat().report(loop, thread_count);

        dataloader.report();

    } catch (const std::filesystem::filesystem_error& e){
        printf("%s\n", e.what());
        return -1;
    }

    return 0;
}
