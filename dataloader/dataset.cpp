#include "dataset.h"
#include "utils.h"

#include <string>
#include <iostream>

#undef DLOG
#define DLOG(...)

ImageFolder::ImageFolder(std::string const& folder_name, bool verbose):
    folder(folder_name)
{
    DLOG("Init Image Folder Main Ctor");

    TimeIt init_time;
    find_all_images(to_path(folder_name));
    double time = init_time.stop();

    if (verbose){
        std::cout << " - " << _images.size() << " images found" << std::endl;
        std::cout << " - " << _classes_to_index.size() << " classes found" << std::endl;
        std::cout << " - took " << time << "s to initialize" << std::endl;
    }
}

void ImageFolder::find_all_images(ImageFolder::Path const& folder_name, int class_index){

    //boost::system::error_code code;
    DLOG("find_all_images");
    auto iterator = FS_NAMESPACE::directory_iterator(folder_name/*, code*/);

    DLOG("Build DB");
    for(FS_NAMESPACE::directory_entry const& entry: iterator){
        if (kw::is_directory(entry)){
            class_index = int(_classes_to_index.size());
            _classes_to_index[FS_NAMESPACE::to_string(entry)] = class_index;

        } else if (kw::is_regular_file(entry) && class_index != -1) {
            _images.push_back({std::string(entry.name), class_index, kw::file_size(entry)});
        } else {
            std::cout << entry.name << " has no class skipping..." << std::endl;
        }
    }
}


ZippedImageFolder::ZippedImageFolder(std::string const& file_name, bool verbose):
    file_name(file_name)
{
    DLOG("Init Image Folder Main Ctor");

    TimeIt init_time;
    int error;
    _zip_handle = zip_open(file_name.c_str(), ZIP_RDONLY, &error);

    find_all_images();
    double time = init_time.stop();

    if (verbose){
        std::cout << " - " << _images.size() << " images found" << std::endl;
        std::cout << " - " << _classes_to_index.size() << " classes found" << std::endl;
        std::cout << " - took " << time << "s to initialize" << std::endl;
    }
}

std::string get_class_name(const std::string& path){
    int count = 0;
    for(char c: path){
        if (c == '/')
            count += 1;
    }

    int pos = 0;
    for(int i = 0; i < path.size(); ++i){
        char c = path[i];

        if (c == '/')
            pos += 1;

        if (pos == count)
            return path.substr(0, i - 1);
    }
}

void ZippedImageFolder::find_all_images(){
    DLOG("find_all_images");
    int entries = zip_get_num_entries(_zip_handle, ZIP_FL_UNCHANGED);

    for(int i = 0; i < entries; ++i){
        zip_stat_t stat;
        zip_stat_index(_zip_handle, i,  ZIP_FL_UNCHANGED, &stat);

        std::string class_name = get_class_name(stat.name);
        int count = _classes_to_index.count(class_name);
        int class_index = 0;

        if (count == 0){
            class_index = _classes_to_index.size();
            _classes_to_index[class_name] = class_index;
        } else {
            class_index = _classes_to_index[class_name];
        }

        // this is a file not a folder
        if (stat.size > 0){
            _images.push_back({stat.name, stat.index, class_index, stat.size});
        }

    }
}
