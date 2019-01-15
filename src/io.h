#ifndef DATALOADER_IO_HEADER_H
#define DATALOADER_IO_HEADER_H

#include "ffilesystem.h"

std::vector<unsigned char> load_file(std::filesystem::path const& file_name, std::size_t file_size);

#endif

