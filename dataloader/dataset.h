#ifndef DATALOADER_IMAGE_FOLDER_HEADER_H
#define DATALOADER_IMAGE_FOLDER_HEADER_H

#include "ffilesystem.h"
#include "image.h"
#include "loader.h"
#include "runtime.h"
#include "utils.h"
#include "io.h"
#include "zip_handles.h"

#include <future>
#include <tuple>
#include <string>
#include <functional>
#include <cassert>

#include <zip.h>

struct DatasetInterface{
    virtual ~DatasetInterface(){}

    virtual Dict<std::string, int> const&   classes_to_label()  const = 0;

    virtual std::tuple<Bytes, int>          get_item(int index) const = 0;

    virtual std::size_t                     size()              const = 0;
};

// Mirror Pytorch ImageFolder
// except it does not do the transformation this should be done in the loader
class ImageFolder: public DatasetInterface{
public:
    struct Sample;

    using Path = FS_NAMESPACE::path;

    ImageFolder (std::string const& folder_name, bool verbose=true);

    Dict<std::string, int> const&   classes_to_label()  const override;

    std::tuple<Bytes, int>          get_item(int index) const override;

    std::size_t                     size()              const override;

    ~ImageFolder();

    Array<Sample> const& samples() const;

    void find_all_images(Path const& folder_name, int class_index=-1);

    // Load files from disk
    Bytes load_file(std::string const& file_name, std::size_t file_size) const;

public:
    std::string const folder;

public:
    struct Sample{
        std::string name;   //! full path to the file
        int label;          //! file label
        std::size_t size;   //! file size
    };

private:
    Array<Sample>           _images;
    Dict<std::string, int>  _classes_to_index;
};


//! Load a Zip file organized as an ImageFolder
class ZippedImageFolder: public DatasetInterface{
public:
    struct Sample;

    ZippedImageFolder(std::string const& file_name, bool verbose=true);

    Dict<std::string, int> const&   classes_to_label()  const override;

    std::tuple<Bytes, int>          get_item(int index) const override;

    std::size_t                     size()              const override;

    ~ZippedImageFolder();

    Array<Sample> const& samples() const;

    void find_all_images();

    // Load a file from a zipped archive
    Bytes load_file(int i, std::size_t file_size) const;

public:
    std::string const file_name;

public:
    struct Sample{
        std::string name; //! full path to the file inside the ZIP
        int index;        //! index of the file inside the ZIP
        int label;        //! Label of the file
        std::size_t size; //! file size
    };

private:
    Array<Sample>           _images;
    Dict<std::string, int>  _classes_to_index;
    mutable ZipHandles      _handles;
};

/*! High Level API for datasets
 *
 * Parameters
 * ----------
 * backend      : name of the dataset backend
 * folder_name  : folder/file to load
 * verbose      : print some information about the dataset
 */
struct Dataset{
    Dataset(const std::string& backend, std::string const& folder_name, bool verbose=true);

    Dict<std::string, int> const&   classes_to_label()  const;

    std::tuple<Bytes, int>          get_item(int index) const;

    std::size_t                     size()              const;

private:
    std::shared_ptr<DatasetInterface> _impl = nullptr;
};

#endif
