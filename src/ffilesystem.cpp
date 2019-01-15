#include "ffilesystem.h"


bool is_directory(std::filesystem::directory_entry const& entry){
    return std::filesystem::is_directory(entry.status());
}

bool is_regular_file(std::filesystem::directory_entry const& entry){
    return std::filesystem::is_regular_file(entry.status());
}

std::size_t file_size(std::filesystem::directory_entry const& entry){
    return std::filesystem::file_size(entry.path());
}
