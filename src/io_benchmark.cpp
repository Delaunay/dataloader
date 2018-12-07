
#include "io.h"
#include "dataset.h"
#include "dataloader.h"
#include "utils.h"

#include <sstream>
#include <cstdio>

using Path = ImageFolder::Path;

int main(int argc, const char* argv[]){

    double total_time = 0;
    int count_image = 0;
    int total_size = 0;

    int image_to_load = 256;
    int seed = 0;
    const char* data_loc = "/media/setepenre/UserData/tmp/fake/";

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

        TimeIt read_time;
        std::string buffer = load_file(path, size);

        total_time += read_time.stop();
        count_image += 1;
        total_size += size;

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

        printf("Read %d images in %.4f sec\n", count_image, total_time);
        printf(" - %.4f file/sec\n", count_image / total_time);
        printf(" - %.4f Mo/sec\n", (total_size / 1024 / 1024) / total_time);
        printf(" - Overhead %.4f sec \n", loop - total_time);
    } catch (const std::filesystem::filesystem_error& e){
        printf("%s\n", e.what());
        return -1;
    }

    return 0;
}
