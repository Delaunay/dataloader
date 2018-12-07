#include "io.h"

#include <cassert>
#include <cstdio>

#define ERROR(msg) std::cout << msg << std::endl

// load a file in one shot into a buffer
std::string load_file(std::filesystem::path const& file_name, std::size_t file_size){
    FILE* file = fopen(file_name.c_str(), "r");
    std::string buffer(file_size, ' ');

    std::size_t read_size = fread(&buffer[0], sizeof(char), file_size, file);
    assert(read_size == file_size && "read_size != file_size");

    fclose(file);
    return buffer;
}
