#include "dataset.h"
#include "dataloader.h"
#include "jpeg.h"
#include "utils.h"
#include "runtime.h"
#include "pool.h"

#include <ctime>
#include <sstream>
#include <cstdio>
#include <cstring>



using Path = ImageFolder::Path;



Image single_threaded_loader(std::tuple<Path, int, std::size_t> const& item){
    Path path; int label; std::size_t size;
    std::tie(path, label, size) = item;

    // Read
    TimeIt read_time;
    auto jpeg = JpegImage(path.c_str(), size);
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
    int thread_count = 16;
    int batch_size = 32;
    int buffering = 3;

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

    //ThreadPool<std::tuple<Path, int, std::size_t> const&, JpegImage> io_pool(2, image_to_load * 2);
    //ThreadPool<std::shared_future<JpegImage>, Image> decode_pool(12, image_to_load * 2);
    Transform trans;
    trans.hflip();

    /*
    std::function<std::shared_future<Image>(std::tuple<Path, int, std::size_t> const& item)> async_loader = [&](std::tuple<Path, int, std::size_t> const& item){
        Path path; int label; std::size_t size;
        std::tie(path, label, size) = item;

        std::optional<std::shared_future<JpegImage>> ojpeg = io_pool.insert_task(item, [&](std::tuple<Path, int, std::size_t> const& item){
            TimeIt read_time;
            auto img = JpegImage(path.c_str(), size);
            RuntimeStats::stat().insert_read(read_time.stop(), size);
            return img;
        });

        if (ojpeg.has_value()){
             std::optional<std::shared_future<Image>> result = decode_pool.insert_task(ojpeg.value(), [&](std::shared_future<JpegImage> fut){
                fut.wait();
                JpegImage jpeg = fut.get();

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
            });

            if (result.has_value())
                return result.value();

            throw std::runtime_error("No Future Image, an error occured");
        }

        throw std::runtime_error("No Future JPEG, an error occured");
    };*/

    try{
        ImageFolder dataset(data_loc, single_threaded_loader);
        // ImageFolder const& dataset, int batch_size, int worker_cout = 6, int buffering=1, int seed=0
        DataLoader dataloader(dataset, batch_size, thread_count, buffering, seed);

        //std::vector<std::shared_future<Image>> images;

        TimeIt loop_time;
        for(int i = 0; i < image_to_load; ++i){
            dataloader.get_next_item();
        }

        /*
        for(auto& future: images){
            future.wait();
        }*/

        //io_pool.shutdown();
        //decode_pool.shutdown();

        double loop = loop_time.stop();
        RuntimeStats::stat().report(loop, thread_count);

        dataloader.report();

        //io_pool.report();
        //decode_pool.report();

    } catch (const std::filesystem::filesystem_error& e){
        printf("%s\n", e.what());
        return -1;
    }

    return 0;
}
