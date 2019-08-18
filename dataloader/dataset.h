#ifndef DATALOADER_IMAGE_FOLDER_HEADER_H
#define DATALOADER_IMAGE_FOLDER_HEADER_H

#include "ffilesystem.h"
#include "image.h"
#include "loader.h"

#include <future>
#include <unordered_map>
#include <tuple>
#include <vector>
#include <string>
#include <functional>
#include <cassert>

#include "utils.h"

#undef DLOG
#define DLOG(...)

// Mirror Pytorch ImageFolder
// except it does not do the transformation this should be done in the loader
class ImageFolder{
public:
    using Path = FS_NAMESPACE::path;
    using Loader = std::function<Image(std::tuple<std::string, int, std::size_t> const& item)>;

    ImageFolder(std::string const& folder_name, Loader const& loader, bool verbose=true);

    ImageFolder(std::string const& folder_name, bool verbose=true):
        ImageFolder(folder_name, single_threaded_loader, verbose)
    {
        DLOG("ImageFolder Delegate Ctor");
    }

    std::tuple<Image, int> get_item(int index) const{
        assert(index >= 0  && std::size_t(index) < _images.size() && "image index out of bounds");
        auto img_ref = _images[std::size_t(index)];
        return std::make_tuple(loader(img_ref), std::get<1>(img_ref));
    }

    std::size_t size() const {
        return _images.size();
    }

    std::vector<std::tuple<std::string, int, std::size_t>> const& samples() const {
        return _images;
    }
    std::unordered_map<std::string, int> const& classes_to_label() const {
        return _classes_to_index;
    }

    Loader loader;
    std::string const folder;
private:

    // Vector[path, label, size]
    std::vector<std::tuple<std::string, int, std::size_t>> _images;
    std::unordered_map<std::string, int> _classes_to_index;

private:
    // folder_name/classe_name/sample
    void find_all_images(Path const& folder_name, int class_index=-1);
};

#endif
