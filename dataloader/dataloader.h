#ifndef DATALOADER_DATALOADER2_HEADER_H
#define DATALOADER_DATALOADER2_HEADER_H

#include "dataset.h"
#include "pool.h"
#include "runtime.h"
#include "io.h"

#include <random>
#include <algorithm>

#include "sampler.h"
#include "utils.h"

#undef DLOG
#define DLOG(...)

template<typename T>
class MappedStorage{
public:
    MappedStorage(T* ptr, int size):
        _data(ptr), _size(size)
    {}

    T  operator[] (int idx) const { return _data[idx]; }
    T& operator[] (int idx)       { return _data[idx]; }

    MappedStorage<T> sub(int start, int size){
        return MappedStorage<T>(_data + start, size);
    }

    void* data(){
        return _data;
    }

    int size() const {  return _size;}

private:
    T* _data;
    int _size;
};



class DataLoader{
public:
    using Transform = std::function<Image(const Bytes&)>;
    using Buffer = std::vector<char>;

    /**
     * @brief DataLoader    Loads dataset samples and accumulate them into batches for training
     * @param dataset       Dataset from which the samples are drawn
     * @param batch_size    How many samples to accumulate
     * @param trans         image transform function Transforms bytes into images
     * @param buffering     How many bathes to work at once
     * @param seed          Seed for the PRNG
     * @param io            Number of IO threads
     */
    DataLoader(const Dataset& dataset,
               const Sampler& sampler,
               int batch_size_,
               Transform trans,
               int worker_cout = 8,
               int buffering_=1,
               int seed=0,
               int io = 0);

    DataLoader(const Dataset& dataset,
               const Sampler& sampler,
               int batch_size_,
               int worker_cout = 8,
               int buffering_=1,
               int seed=0,
               int io = 0):
        DataLoader(dataset, sampler, batch_size_, single_threaded_loader, worker_cout, buffering_, seed, io)
    {}

    ~DataLoader(){
        pool.shutdown(true);
    }

    //! Return the image size in bytes
    std::size_t image_size(){
        return 3 * 224 * 224;
    }

    //! Notify that the image is loaded and ready
    void mark_ready(int idx, int label){
        image_ready[idx] = label;
    }

    //! Notify that the image was consumed by the batch reduce
    void mark_empty(int idx){
        image_ready[idx] = -1;
    }

    //! check if an image is loaded and ready to be consumed
    bool is_ready(int idx) const {
        return image_ready[idx] != -1;
    }

    //! Returns the underlying memory used for an image
    //! You should use `is_ready` to make sure the image data is populated
    MappedStorage<uint8_t> image_mem(int idx){
        assert(idx >= 0 && idx < batch_size * buffering);
        int size = image_size();
        return MappedStorage<uint8_t>(memory_pool.data() + idx * size, size);
    }

    //! Returns batch storage idx being the buffering index
    MappedStorage<uint8_t> batch_mem(int idx){
        assert(idx >= 0 && idx < buffering);
        int size = image_size() * batch_size;
        return MappedStorage<uint8_t>(memory_pool.data() + idx * size, size);
    }

    //! Queue the loading of our next batch
    void send_next_batch();

    //! Get current batch and queue next batch
    std::tuple<std::vector<uint8_t>, std::vector<int>> get_next_item();

    //! Get current batch without queuing future batch
    std::tuple<std::vector<uint8_t>, std::vector<int>> get_future_batch();

    void report() const {
        double loop = loop_time.stop();
        RuntimeStats::stat().report(loop, workers, io_threads);
        pool.report();
    }

    void shutdown(){
        pool.shutdown(true);
    }

    std::size_t epoch() const {
        return sampler.epoch();
    }

public:
    const Dataset dataset;
    Sampler   sampler;
    int const buffering;
    int const batch_size;
    int const workers;
    int const io_threads;
    int const seed;
    Transform trans;

private:
    ThreadPool<std::tuple<int, int>, bool> pool;
    std::mutex _lock;
    // little allocation is done most of the memory is allocated once and reused
    std::vector<uint8_t> memory_pool;

    // Is the image ready, used as image lock
    // the loader set the flag to true once the image is ready
    // the loader pool the flag until it is ready
    // there is no concurrent write in the image index so we do not need to lock it
    // cant be a vector<bool> because then the update is not atomic
    std::vector<int>  image_ready;

    std::size_t sent_batch = 0;
    std::size_t retrieve_batch = 0;

    TimeIt loop_time;
};

#endif
