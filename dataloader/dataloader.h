#ifndef DATALOADER_DATALOADER2_HEADER_H
#define DATALOADER_DATALOADER2_HEADER_H

#include "dataset.h"
#include "pool.h"
#include "runtime.h"
#include "io.h"

#include <random>
#include <algorithm>

#include "utils.h"

#undef DLOG
#define DLOG(...)

template<typename T>
class MappedStorage{
public:
    MappedStorage(T* ptr, int size):
        _data(ptr), _size(size)
    {
        //DLOG("Mapped Storage %i", size);
    }

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
    using Path = FS_NAMESPACE::path;

    /**
     * @brief DataLoader    Loads dataset samples and accumulate them into batches for training
     * @param dataset       Dataset from which the samples are drawn
     * @param batch_size    How many samples to accumulate
     * @param buffering     How many bathes to work at once
     * @param seed          Seed for the PRNG
     */
    DataLoader(ImageFolder const& dataset, int batch_size_, int worker_cout = 8, int buffering_=1, int seed=0, int io = 0):
        dataset(dataset), buffering(buffering_), batch_size(batch_size_),
        workers(worker_cout), io_threads(io), seed(seed), pool(worker_cout, batch_size_ * buffering_)
    {
        DLOG("%s", "init dataloader");

        if (io == 0){
            io = worker_cout;
        }

        make_io_lock(io);

        image_indices = std::vector<std::size_t>(dataset.size());

        for(std::size_t i = 0; i < std::size_t(dataset.size()); ++i){
            image_indices[i] = i;
        }

        shuffle();

        memory_pool = std::vector<uint8_t>(batch_size_ * image_size() * buffering_);
        image_ready = std::vector<int>(batch_size_ * buffering_, -1);

        for(int i = 0; i < buffering_; ++i){
            DLOG("%s", "send batch request");
            send_next_batch();
        }
    }

    std::size_t image_size(){
        return 3 * 224 * 224;
    }

    // return the underlying memory used for image idx
    MappedStorage<uint8_t> image_mem(int idx){
        //DLOG("image_mem (idx: %d) (max: %d)", idx, batch_size * buffering);
        assert(idx >= 0 && idx < batch_size * buffering);

        int size = image_size();
        return MappedStorage<uint8_t>(memory_pool.data() + idx * size, size);
    }

    void mark_ready(int idx, int label){
        image_ready[idx] = label;
    }

    void mark_empty(int idx){
        image_ready[idx] = -1;
    }

    bool is_ready(int idx) const {
        return image_ready[idx] != -1;
    }

    MappedStorage<uint8_t> batch_mem(int idx){
        assert(idx >= 0 && idx < buffering);
        int size = image_size() * batch_size;
        return MappedStorage<uint8_t>(memory_pool.data() + idx * size, size);
    }

    std::size_t get_next_image_index();

    std::size_t epoch() const {
        return _epoch;
    }

    void send_next_batch();

    std::tuple<std::vector<uint8_t>, std::vector<int>> get_next_item();

    ~DataLoader(){
        pool.shutdown();
    }

    ImageFolder const& dataset;
    int const buffering;
    int const batch_size;
    int const workers;
    int const io_threads;
    int const seed;

    void report() const {
        double loop = loop_time.stop();
        RuntimeStats::stat().report(loop, workers, io_threads);
        pool.report();
    }

    void shutdown(){
        pool.shutdown();
    }

    std::tuple<std::vector<uint8_t>, std::vector<int>> get_future_batch();

    void shuffle();

private:
    using Buffer = std::vector<char>;
    ThreadPool<std::tuple<int, int>, bool> pool;
    std::mutex _lock;

    // shuffuled
    std::vector<std::size_t> image_indices{dataset.size()};

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

    std::mt19937 prng_engine{seed};
    std::size_t image_iterator = 0;

    bool reshuffle_after_epoch = false;
    std::size_t _epoch = 0;
    TimeIt loop_time;
};

#endif
