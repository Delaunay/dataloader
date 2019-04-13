#ifndef DATALOADER_FFILESYSTEM_HEADER_H
#define DATALOADER_FFILESYSTEM_HEADER_H

#include <string>
#include <vector>
#include <cstddef>

#include "utils.h"

#ifdef USE_CPP
#if defined(__has_include)
#   if __has_include(<filesystem>)
#       define USE_STD_FS
#       include <filesystem>
#       define FS_NAMESPACE std::filesystem
#   else
#       define USE_BOOST_FS
#       include <boost/filesystem.hpp>
#       include <boost/system/error_code.hpp>
#       define FS_NAMESPACE boost::filesystem
#   endif
#endif


namespace kw{
bool is_directory(FS_NAMESPACE::directory_entry const& entry);
bool is_regular_file(FS_NAMESPACE::directory_entry const& entry);
std::size_t file_size(FS_NAMESPACE::directory_entry const& entry);
}
#else

#define FS_NAMESPACE cfs

namespace cfs {
struct Entry{
    Entry(std::string const& name_, bool is_file_, std::size_t size_):
        name(name_), is_file(is_file_), size(size_)
    {
        //DLOG(name.c_str());
    }

    Entry() = default;

    std::string name = "";
    bool is_file = false;
    std::size_t size = 0;
};

using path = char const*;

using directory_entry = Entry;

std::vector<directory_entry> directory_iterator(path p);

inline
std::string to_string(directory_entry path){
    return path.name;
}
}

inline
cfs::path to_path(std::string const& val){
    return val.c_str();
}

inline
cfs::path to_cstr(std::string const& val){
    return val.c_str();
}

namespace kw{
inline
bool is_directory(cfs::directory_entry entry) { return !entry.is_file; }
inline
bool is_regular_file(cfs::directory_entry entry) { return entry.is_file; }
inline
std::size_t file_size(cfs::directory_entry entry) { return entry.size; }
}

#endif
#endif
