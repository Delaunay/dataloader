#ifndef DATALOADER_FFILESYSTEM_HEADER_H
#define DATALOADER_FFILESYSTEM_HEADER_H

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

#endif
