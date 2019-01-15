#ifndef DATALOADER_FFILESYSTEM_HEADER_H
#define DATALOADER_FFILESYSTEM_HEADER_H

#if defined(__has_include)
#   if __has_include(<filesystem>)
#       include <filesystem>
#   else
#       include <boost/filesystem.hpp>
        namespace std{
            namespace filesystem = boost::filesystem;
        }
/*
#       include <experimental/filesystem>
        namespace std{
            namespace filesystem = std::experimental::filesystem;
        }
*/
#   endif
#endif

bool is_directory(std::filesystem::directory_entry const& entry);
bool is_regular_file(std::filesystem::directory_entry const& entry);
std::size_t file_size(std::filesystem::directory_entry const& entry);

#endif
