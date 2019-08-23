#ifndef ZIP_HANDLES_HEADER_H
#define ZIP_HANDLES_HEADER_H

#include "utils.h"

#include <mutex>
#include <vector>

#include <zip.h>

/*!
 * \brief The ZipHandles class is used as a pool of zip handles.
 * because libzip is not thread safe so each threads needs its own handles.
 * Creating handles is quite slow as it needs to read the index.
 * So we use this as pool of handles that get reused.
 */
class ZipHandles{
private:
    struct Item{
        zip_t* handle;
        bool free = false;
        int index = 0;
    };

public:
    // Sinple context mananger, free up the handle once going out of scope
    struct ZipHandle{
        ZipHandle(Item handle, ZipHandles* parent):
            handle(handle), parent(parent)
        {}

        operator zip_t*(){
            return handle.handle;
        }

        ~ZipHandle(){
            parent->free_handle(handle.index);
        }

        Item handle;
        ZipHandles* parent;
    };

public:
    ZipHandles(std::string const& file_name, int size=12):
        file_name(file_name)
    {
        for(int i = 0; i < size; ++i){
             make_handle();
        }
    }

    ZipHandle get_zip(){
        std::lock_guard lock(mutex);

        for(int i = 0, n = int(handles.size()); i < n; ++i){
            Item& item = handles[i];
            if (item.free){
                // DLOG("use handle %d", i);
                item.index = i;
                item.free = false;
                return ZipHandle(item, this);
            }
        }

        DLOG("Create a new zip handle %d", int(handles.size()));
        int error = 0;
        zip_t* data = zip_open(file_name.c_str(), ZIP_RDONLY, &error);
        Item item{data, false, int(handles.size())};
        handles.push_back(item);

        return ZipHandle(item, this);
    }

    void make_handle(){
        DLOG("Create a new zip handle %d", int(handles.size()));
        int error = 0;
        zip_t* data = zip_open(file_name.c_str(), ZIP_RDONLY, &error);
        Item item{data, true, int(handles.size())};
        handles.push_back(item);
    }

    void free_handle(int idx){
        // if we do not lock we risk the vector to resize in between
        // DLOG("release handle %d", idx);
        std::lock_guard lock(mutex);
        handles[idx].free = true;
    }

    ~ZipHandles(){
        for(Item& handle: handles){
            zip_close(handle.handle);
        }
    }

private:
    std::string file_name;
    std::mutex mutex;
    std::vector<Item> handles;
};

#endif
