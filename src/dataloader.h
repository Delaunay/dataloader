#ifndef DATALOADER_DATALOADER_HEADER_H
#define DATALOADER_DATALOADER_HEADER_H

#include "dataset.h"
#include "pool.h"
#include "runtime.h"

#include <random>
#include <algorithm>

class DataLoader{
public:
    using Path = std::filesystem::path;

    /**
     * @brief DataLoader    Loads dataset samples and accumulate them into batches for training
     * @param dataset       Dataset from which the samples are drawn
     * @param batch_size    How many samples to accumulate
     * @param buffering     How many bathes to work at once
     * @param seed          Seed for the PRNG
     */
    DataLoader(ImageFolder const& dataset, std::size_t batch_size_, std::size_t worker_cout = 6, std::size_t buffering_=1, int seed=0):
        dataset(dataset), buffering(buffering_), batch_size(batch_size_), seed(seed), pool(worker_cout, batch_size_ * buffering_)
    {
        image_indices = std::vector<std::size_t>(dataset.size());

        for(std::size_t i = 0; i < std::size_t(dataset.size()); ++i){
            image_indices[i] = i;
        }

        shuffle();

        future_buffered_batch = std::vector<std::vector<std::shared_future<Image>>>(buffering);

        for(std::size_t i = 0; i < buffering; ++i){
           future_buffered_batch[i] = std::vector<std::shared_future<Image>>(batch_size);
        }

        for(std::size_t i = 1; i < buffering; ++i){
           send_next_batch();
        }
    }

    std::size_t get_next_image_index(){
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


    void send_next_batch(){
        std::vector<std::shared_future<Image>>& future_batch = future_buffered_batch[buffering_index];

        TimeIt schedule_time;
        for(std::size_t i = 0; i < batch_size;){
            int index = int(get_next_image_index());

            auto val = pool.insert_task(index, [&](int index){
                return dataset.get_item(index);
            });

            if (val.has_value()){
                future_batch[i] = val.value();
                i += 1;
            } else {
                printf("Error image skipped (full: %d, size: %lu)\n", pool.is_full(), pool.size());
            }
        }
        RuntimeStats::stat().insert_schedule(schedule_time.stop(), batch_size);
        buffering_index = (buffering_index + 1) % buffering;
    }

    // should return a batch not an image
    std::vector<Image> get_next_item(){
        std::vector<Image> batch;
        batch.reserve(batch_size);

        send_next_batch();
        std::vector<std::shared_future<Image>>& future_batch = future_buffered_batch[next_batch];
        next_batch = (next_batch + 1) % buffering;

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
    std::size_t const buffering;
    std::size_t const batch_size;
    int const seed;

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

    std::vector<std::vector<std::shared_future<Image>>> future_buffered_batch;
    std::size_t buffering_index = 0;
    std::size_t next_batch = 0;

    std::mt19937 prng_engine{seed};
    std::size_t image_iterator = 0;

    bool reshuffle_after_epoch = false;
    std::size_t _epoch = 0;
};

#endif
