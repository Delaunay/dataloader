#ifndef DATALOADER_STH_LOADER_HEADER_H
#define DATALOADER_STH_LOADER_HEADER_H

#include "image.h"
#include "ffilesystem.h"
#include "utils.h"

Image single_threaded_loader(const Bytes& data);

#endif
