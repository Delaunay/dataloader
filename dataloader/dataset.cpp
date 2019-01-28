#include "dataset.h"
#include "utils.h"

#include <string>
#include <iostream>

namespace fs = std::filesystem;

ImageFolder::ImageFolder(std::string const& folder_name, ImageFolder::Loader const& loader, bool verbose):
    loader(loader), folder(folder_name)
{
    TimeIt init_time;
    std::cout << folder_name << std::endl;

    ImageFolder::Path path = folder_name;
    find_all_images(path);
    double time = init_time.stop();

    if (verbose){
        std::cout << " - " << _images.size() << " images found" << std::endl;
        std::cout << " - " << _classes_to_index.size() << " classes found" << std::endl;
        std::cout << " - took " << time << "s to initialize" << std::endl;
    }
}

void ImageFolder::find_all_images(ImageFolder::Path const& folder_name, int class_index){

    auto iterator = fs::directory_iterator(folder_name);

    DLOG("Iterator Created");

    for(fs::directory_entry const& entry: iterator){
        if (is_directory(entry)){
            int index = int(_classes_to_index.size());

            _classes_to_index[entry.path().string()] = index;
            find_all_images(entry, index);

        } else if (is_regular_file(entry) && class_index != -1) {
            _images.emplace_back(entry, class_index, file_size(entry));
        } else {
            std::cout << entry << " has no class skipping..." << std::endl;
        }
    }
}
