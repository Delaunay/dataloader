#ifndef DATALOADER_IO_HEADER_H
#define DATALOADER_IO_HEADER_H

#include "ffilesystem.h"
#include "utils.h"

std::vector<unsigned char> load_file(std::filesystem::path const& file_name, std::size_t file_size);

Semaphore& make_io_lock(std::size_t max_io_thread);

void start_io();

void end_io();

class IOGuardLock{
    IOGuardLock(){
        start_io();
    }

    ~IOGuardLock(){
        end_io();
    }
};


#endif

