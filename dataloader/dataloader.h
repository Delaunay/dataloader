#ifndef DATALOADER_DATALOADER_HEADER_H
#define DATALOADER_DATALOADER_HEADER_H

#include "dataset.h"
#include "pool.h"
#include "runtime.h"
#include "io.h"

#include <random>
#include <algorithm>

#include "utils.h"

#undef DLOG
#define DLOG(...)

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
    DataLoader(ImageFolder const& dataset, std::size_t batch_size_, std::size_t worker_cout = 6, std::size_t buffering_=1, int seed=0, std::size_t io = 0);

    std::size_t get_next_image_index();

    std::size_t epoch() const {
        return _epoch;
    }

    void send_next_batch();

    std::vector<Image> get_next_item();

    ~DataLoader(){
        pool.shutdown();
    }

    ImageFolder const& dataset;
    std::size_t const buffering;
    std::size_t const batch_size;
    int const seed;

    void report() const {
        pool.report();
    }

    void shutdown(){
        pool.shutdown();
    }

    std::vector<Image> get_future_batch();

    std::vector<Image> reduce_to_vector(std::vector<Image> const& future_batch);

    void shuffle();

private:
    using Buffer = std::vector<char>;
    ThreadPool<int, Image> pool;
    std::mutex _lock;

    std::vector<std::size_t> image_indices{dataset.size()};

    std::vector<std::vector<std::shared_future<Image>>> future_buffered_batch;
    std::size_t buffering_index = 0;
    std::size_t next_batch = 0;

    std::mt19937 prng_engine{seed};
    std::size_t image_iterator = 0;

    bool reshuffle_after_epoch = false;
    std::size_t _epoch = 0;
};

#endif
