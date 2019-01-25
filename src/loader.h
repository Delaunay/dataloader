#ifndef DATALOADER_STH_LOADER_HEADER_H
#define DATALOADER_STH_LOADER_HEADER_H

#include "image.h"
#include "ffilesystem.h"

Image single_threaded_loader(std::tuple<std::filesystem::path, int, std::size_t> const& item);

#endif
