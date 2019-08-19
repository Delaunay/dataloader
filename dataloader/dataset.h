#ifndef DATALOADER_IMAGE_FOLDER_HEADER_H
#define DATALOADER_IMAGE_FOLDER_HEADER_H

#include "ffilesystem.h"
#include "image.h"
#include "loader.h"
#include "runtime.h"
#include "utils.h"
#include "io.h"

#include <future>
#include <tuple>
#include <string>
#include <functional>
#include <cassert>

#include <zip.h>

#undef DLOG
#define DLOG(...)

// Mirror Pytorch ImageFolder
// except it does not do the transformation this should be done in the loader
class ImageFolder{
public:
    using Path = FS_NAMESPACE::path;

    struct Sample{
        std::string name;
        int label;
        std::size_t size;
    };

    ImageFolder(std::string const& folder_name, bool verbose=true);

    std::tuple<Bytes, int> get_item(int index) const {
        assert(index >= 0  && std::size_t(index) < _images.size() && "image index out of bounds");
        auto img_ref = _images[std::size_t(index)];

        Bytes data = load_file(img_ref.name, img_ref.size);
        return std::make_tuple(data, img_ref.label);
    }

    std::size_t size() const {
        return _images.size();
    }

    Array<Sample> const& samples() const {
        return _images;
    }
    Dict<std::string, int> const& classes_to_label() const {
        return _classes_to_index;
    }

    Bytes load_file(std::string const& file_name, std::size_t file_size) const {
        Bytes buffer(file_size);

        DLOG("%s", "Waiting for IO resource");
        TimeIt io_block_time;
        start_io();
        RuntimeStats::stat().insert_io_block(io_block_time.stop());

        TimeIt read_time;
        FILE* file = fopen(file_name.c_str(), "r");
        std::size_t read_size = fread(&buffer[0], sizeof(unsigned char), file_size, file);
        RuntimeStats::stat().insert_read(read_time.stop(), file_size);

        end_io();

        assert(read_size == file_size && "read_size != file_size");
        fclose(file);
        return buffer;
    }

    std::string const folder;
private:

    // Vector[path, label, size]
    Array<Sample> _images;
    Dict<std::string, int> _classes_to_index;

private:
    // folder_name/classe_name/sample
    void find_all_images(Path const& folder_name, int class_index=-1);
};


class ZippedImageFolder{
public:
    struct Sample{
        std::string name;
        int index;
        int label;
        std::size_t size;
    };

    ZippedImageFolder(std::string const& file_name, bool verbose=true);

    std::tuple<Bytes, int> get_item(int index) const {
        assert(index >= 0 && std::size_t(index) < _images.size() && "image index out of bounds");
        const Sample& img_ref = _images[std::size_t(index)];

        Bytes data = load_file(img_ref.index, img_ref.size);
        return std::make_tuple(data, img_ref.label);
    }

    int size() const {
        return int(_images.size());
    }

    Array<Sample> const& samples() const {
        return _images;
    }
    Dict<std::string, int> const& classes_to_label() const {
        return _classes_to_index;
    }

    //! load a file from a zipped archive
    Bytes load_file(int i, std::size_t file_size) const {
        DLOG("%s", "Waiting for IO resource");
        Bytes buffer(file_size);

        TimeIt io_block_time;
        start_io();
        RuntimeStats::stat().insert_io_block(io_block_time.stop());

        TimeIt read_time;
        zip_file_t* file_h = zip_fopen_index(_zip_handle, i, ZIP_FL_UNCHANGED);
        std::size_t read_size = zip_fread(file_h, &buffer[0], file_size);
        RuntimeStats::stat().insert_read(read_time.stop(), file_size);

        end_io();

        assert(read_size == file_size && "read_size != file_size");
        zip_fclose(file_h);
        return buffer;
    }

    std::string const file_name;
private:

    // Vector[path, label, size]
    Array<Sample> _images;
    Dict<std::string, int> _classes_to_index;

    zip_t* _zip_handle = nullptr;

private:
    // folder_name/classe_name/sample
    void find_all_images();
};



#endif
