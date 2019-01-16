#include "dataset.h"
#include "dataloader.h"
#include "jpeg.h"
#include "utils.h"
#include "runtime.h"

#include <ctime>
#include <sstream>
#include <cstdio>
#include <cstring>

using Path = ImageFolder::Path;



int main(int argc, const char* argv[]){

    int image_to_load = 2048;
    int seed = int(time(nullptr));

    const char* data_loc = "/home/user1/test_database/imgnet/ImageNet2012_jpeg/train/";
    //const char* data_loc = "/media/setepenre/UserData/tmp/jpeg";

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
    }

    std::function<int(std::tuple<Path, int, std::size_t> const& item)> bench_fun = [&](std::tuple<Path, int, std::size_t> const& item){

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

        //img.save_ppm("after_decode.ppm");

        // Scale
        TimeIt scale_time;
        img.inplace_scale(224, 224);
        RuntimeStats::stat().insert_scaling(scale_time.stop(), img.size());

        RuntimeStats::stat().increment_count();

        //img.save_ppm("after_scaling.ppm");

        return 0;
    };

    try{
        ImageFolder dataset(data_loc, bench_fun);
        DataLoader dataloader(dataset, 1, 1, seed);

        TimeIt loop_time;
        for(int i = 0; i < image_to_load; ++i){
            dataloader.get_next_item();
        }
        double loop = loop_time.stop();

        RuntimeStats::stat().report(loop);

    } catch (const std::filesystem::filesystem_error& e){
        printf("%s\n", e.what());
        return -1;
    }

    return 0;
}
