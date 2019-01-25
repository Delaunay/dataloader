#include "io.h"
#include "dataset.h"
#include "dataloader.h"
#include "utils.h"

#include <iostream>


int main(){
    using Path = std::filesystem::path;
    double total = 0;
    int count = 0;
    int total_size = 0;
    /*

    std::function<int(std::tuple<Path, int, std::size_t> const& item)> loader_fun = [&](std::tuple<Path, int, std::size_t> const& item){
        Path path; int label; std::size_t size;
        std::tie(path, label, size) = item;
        std::cout << path << " " << label << " " << size << std::endl;

        TimeIt read_time;
        auto buffer = load_file(path, size);

        total += read_time.stop();
        count += 1;
        total_size += size;

        //std::cout << buffer;
        return 0;
    };

    ImageFolder dataset("/media/setepenre/UserData/tmp/fake/", loader_fun);

    for(int i = 0; i < 10; ++i){
        auto image = dataset.get_item(i);
    }

    std::cout << "---\n";
    DataLoader loader(dataset, 32, 1, 0);

    for(int i = 0; i < 256; ++i){
        auto image = loader.get_next_item();
    }

    std::cout << " Read " << count << " images in " << total << std::endl;
    std::cout << count / total << " images / sec" << std::endl;
    std::cout << (total_size / 1024 / 1024) / total  << " Mo / sec " << std::endl;*/

    return 0;
}
