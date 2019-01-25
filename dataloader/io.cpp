#include "io.h"

#include <cassert>
#include <cstdio>

#define ERROR(msg) std::cout << msg << std::endl

// load a file in one shot into a buffer
std::vector<unsigned char> load_file(std::filesystem::path const& file_name, std::size_t file_size){
    FILE* file = fopen(file_name.c_str(), "r");
    std::vector<unsigned char> buffer(file_size);

    std::size_t read_size = fread(&buffer[0], sizeof(unsigned char), file_size, file);

    assert(read_size == file_size && "read_size != file_size");

    fclose(file);
    return buffer;
}


Semaphore& make_io_lock(std::size_t max_io_thread){
    static Semaphore io_lock(max_io_thread > 0 ? max_io_thread: 20000);
    return io_lock;
}


void start_io(){
    make_io_lock(0).wait();
}


void end_io(){
    make_io_lock(0).notify();
}
