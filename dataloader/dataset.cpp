#include "dataset.h"
#include "utils.h"

#include <string>
#include <iostream>



ImageFolder::ImageFolder(std::string const& folder_name, ImageFolder::Loader const& loader, bool verbose):
    loader(loader), folder(folder_name)
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

    //boost::system::error_code code;
    auto iterator = FS_NAMESPACE::directory_iterator(folder_name/*, code*/);

    //if (code.value() != 0){
    //    DLOG(code.message().c_str());
    //}

    DLOG("Iterator Created");

    for(FS_NAMESPACE::directory_entry const& entry: iterator){
        if (kw::is_directory(entry)){
            int index = int(_classes_to_index.size());

            _classes_to_index[entry.path().string()] = index;
            find_all_images(entry, index);

        } else if (kw::is_regular_file(entry) && class_index != -1) {
            _images.emplace_back(entry, class_index, kw::file_size(entry));
        } else {
            std::cout << entry << " has no class skipping..." << std::endl;
        }
    }
}
