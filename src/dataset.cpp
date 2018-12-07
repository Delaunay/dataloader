#include "dataset.h"
#include "utils.h"

#include <iostream>


ImageFolder::ImageFolder(std::string const& folder_name, ImageFolder::Loader& loader, bool verbose):
    folder(folder_name), loader(loader)
{
    TimeIt init_time;
    find_all_images(folder_name);
    double time = init_time.stop();

    if (verbose){
        std::cout << " - " << _images.size() << " images found" << std::endl;
        std::cout << " - " << _classes_to_index.size() << " classes found" << std::endl;
        std::cout << " - took " << time << "s to initialize" << std::endl;
    }
}

void ImageFolder::find_all_images(ImageFolder::Path const& folder_name, int class_index){
    namespace fs = std::filesystem;

    for(fs::directory_entry const& entry: fs::directory_iterator(folder_name)){
        if (entry.is_directory()){
            int index = _classes_to_index.size();

            _classes_to_index[entry.path().string()] = index;
            find_all_images(entry, index);

        } else if (entry.is_regular_file() && class_index != -1) {
            _images.emplace_back(entry, class_index, entry.file_size());
        } else {
            std::cout << entry << " has no class skipping..." << std::endl;
        }
    }
}
