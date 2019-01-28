#include "ffilesystem.h"

namespace kw{

bool is_directory(FS_NAMESPACE::directory_entry const& entry){
#ifdef USE_BOOST_FS
    return FS_NAMESPACE::is_directory(entry.status());
#else
    return entry.is_directory();
#endif
}

bool is_regular_file(FS_NAMESPACE::directory_entry const& entry){
#ifdef USE_BOOST_FS
    return FS_NAMESPACE::is_regular_file(entry.status());
#else
    return entry.is_regular_file();
#endif
}

std::size_t file_size(FS_NAMESPACE::directory_entry const& entry){
#ifdef USE_BOOST_FS
    return FS_NAMESPACE::file_size(entry.path());
#else
    return entry.file_size();
#endif
}

}
