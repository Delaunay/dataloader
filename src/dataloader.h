#ifndef DATALOADER_DATALOADER_HEADER_H
#define DATALOADER_DATALOADER_HEADER_H

#include "dataset.h"
#include "pool.h"
#include "runtime.h"

#include <random>
#include <algorithm>

class DataLoader{
public:
    using Batch = int;
    using Sample = int;
    using Path = std::filesystem::path;

    /**
     * @brief DataLoader    Loads dataset samples and accumulate them into batches for training
     * @param dataset       Dataset from which the samples are drawn
     * @param batch_size    How many samples to accumulate
     * @param buffering     How many bathes to work at once
     * @param seed          Seed for the PRNG
     */
    DataLoader(ImageFolder const& dataset, std::size_t batch_size, std::size_t worker_cout = 6, std::size_t buffering=1, int seed=0):
        dataset(dataset), batch_size(batch_size), seed(seed), buffering(buffering), pool(worker_cout, batch_size)
    {
        image_indices = std::vector<std::size_t>(dataset.size());

        for(std::size_t i = 0; i < std::size_t(dataset.size()); ++i){
            image_indices[i] = i;
        }

        shuffle();
    }

    std::size_t get_next_image(){
        std::size_t i = image_iterator;
        image_iterator += 1;

        // start new epoch
        if (image_iterator >= dataset.size()){
            _epoch += 1;
            image_iterator = 0;
            if (reshuffle_after_epoch){
                shuffle();
            }
        }

        // return std::max(prng(prng_engine) - 1, 0);
        return image_indices[i];
    }

    std::size_t epoch() const {
        return _epoch;
    }

    // should return a batch not an image
    std::vector<Image> get_next_item(){
        std::vector<std::shared_future<Image>> future_batch(batch_size);
        std::vector<Image> batch;
        batch.reserve(batch_size);

        TimeIt schedule_time;
        for(std::size_t i = 0; i < batch_size;){
            int index = int(get_next_image());

            auto val = pool.insert_task(index, [&](int index){
                return dataset.get_item(index);
            });

            if (val.has_value()){
                future_batch[i] = val.value();
                i += 1;
            } else {
                printf("Error image skipped\n");
            }
        }
        RuntimeStats::stat().insert_schedule(schedule_time.stop(), batch_size);

        TimeIt batch_time;
        for(std::size_t i = 0; i < batch_size; ++i){
            std::shared_future<Image>& fut = future_batch[i];
            fut.wait();
        }
        RuntimeStats::stat().insert_batch(batch_time.stop(), batch_size);

        TimeIt reduce_time;
        for(std::size_t i = 0; i < batch_size; ++i){
            std::shared_future<Image>& fut = future_batch[i];
            batch.push_back(std::move(fut.get()));
        }
        RuntimeStats::stat().insert_reduce(reduce_time.stop(), batch_size);

        return batch;
    }

    ~DataLoader(){
        pool.shutdown();
    }

    ImageFolder const& dataset;
    std::size_t const batch_size;
    int const seed;
    std::size_t const buffering;

    void report(){
        pool.report();
    }

private:

    void shuffle(){
        auto rand = [this](std::size_t n) -> std::size_t {
            std::uniform_int_distribution<std::size_t> a(0, n);
            return a(prng_engine);
        };

        std::random_shuffle(
            std::begin(image_indices),
            std::end(image_indices),
            rand
        );
    }

private:
    using Buffer = std::vector<char>;
    ThreadPool<int, Image> pool;

    std::vector<std::size_t> image_indices{dataset.size()};
    std::mt19937 prng_engine{seed};
    std::size_t image_iterator = 0;

    bool reshuffle_after_epoch = false;
    std::size_t _epoch = 0;
};

#endif
