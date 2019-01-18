#ifndef DATALOADER_DATALOADER_HEADER_H
#define DATALOADER_DATALOADER_HEADER_H

#include "dataset.h"
#include "pool.h"

#include <random>

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
    DataLoader(ImageFolder const& dataset, int batch_size, int worker_cout = 6, int buffering=1, int seed=0):
        dataset(dataset), batch_size(batch_size), seed(seed), buffering(buffering), pool(worker_cout, batch_size)
    {
    }

    // should return a batch not an image
    std::vector<Image> get_next_item(){
        std::vector<std::shared_future<Image>> future_batch(batch_size);
        std::vector<Image> batch;
        batch.reserve(batch_size);

        for(int i = 0; i < batch_size; ++i){
            int index = std::max(prng(prng_engine) - 1, 0);

            auto val = pool.insert_task(index, [&](int index){
                return dataset.get_item(index);
            });

            if (val.has_value()){
                future_batch[i] = val.value();
            } else {
                // printf("error");
            }
        }

        for(int i = 0; i < batch_size; ++i){
            std::shared_future<Image>& fut = future_batch[i];
            fut.wait();

            batch.push_back(std::move(fut.get()));
        }

        return batch;
    }

    ~DataLoader(){
        pool.shutdown();
    }

    Sample load_sample(Path const& path, int label){

    }


    Batch accumulate_samples(std::vector<int>){

    }
/*
    Batch make_batch(){
        std::vector<std::future<>> samples(batch_size);

        for(){
            samples[i] = load_sample();
        }

        return accumulate_samples(samples);
    }*/

    ImageFolder const& dataset;
    std::size_t const batch_size;
    int const seed;
    std::size_t const buffering;

    void report(){
        pool.report();
    }

private:
    using Buffer = std::vector<char>;
    ThreadPool<int, Image> pool;
    //using Image = std::vector<int8_t>;

    std::mt19937 prng_engine{seed};
    std::uniform_int_distribution<> prng{0, dataset.size()};

    // Ring buffer for
    //  * the io loop to read from
    //std::vector<std::vector<int>> io_work_item{buffering, std::vector<int>(batch_size)};

    // Ring Buffer for
    // * IO Thread to write to
    // * TurboJPEG Thread to read from
    //std::vector<std::vector<Buffer>> jpeg_buffer{buffering, std::vector<Buffer>(batch_size)};

    // Ring Buffer for
    // * TurboJPEG to write to
    //std::vector<std::vector<Image>> batch_buffer{buffering, std::vector<Image>(batch_size)};
};

#endif
