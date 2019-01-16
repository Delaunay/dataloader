#include "ffilesystem.h"


bool is_directory(std::filesystem::directory_entry const& entry){
#ifdef USE_BOOST_FS
    return std::filesystem::is_directory(entry.status());
#else
    return entry.is_directory();
#endif
}

bool is_regular_file(std::filesystem::directory_entry const& entry){
#ifdef USE_BOOST_FS
    return std::filesystem::is_regular_file(entry.status());
#else
    return entry.is_regular_file();
#endif
}

std::size_t file_size(std::filesystem::directory_entry const& entry){
#ifdef USE_BOOST_FS
    return std::filesystem::file_size(entry.path());
#else
    return entry.file_size();
#endif
}
