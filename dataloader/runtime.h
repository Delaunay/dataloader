#ifndef DATALOADER_RUNTIME_HEADER_H
#define DATALOADER_RUNTIME_HEADER_H

#include <cstdio>
#include <atomic>

struct RuntimeStats{
public:
    static RuntimeStats& stat(){
        static RuntimeStats st;
        return st;
    }
#define DEF_ADD_SIZE_PAIR_VAL(name)\
    void insert_##name(double time, std::size_t size){\
        total_time_##name.store(total_time_##name.load() + time);\
        total_size_##name.store(total_size_##name.load() + size);\
    }\
    void insert_##name(double time, int size){\
        total_time_##name.store(total_time_##name.load() + time);\
        total_size_##name.store(total_size_##name.load() + std::size_t(size));\
    }

#define DEF_TIME_SIZE_PAIR(name)\
    std::atomic<double>         total_time_##name{0};\
    std::atomic<std::size_t>    total_size_##name{0};

    DEF_TIME_SIZE_PAIR(read)
    DEF_ADD_SIZE_PAIR_VAL(read)

    DEF_TIME_SIZE_PAIR(transform)
    DEF_ADD_SIZE_PAIR_VAL(transform)

    DEF_TIME_SIZE_PAIR(decode)
    DEF_ADD_SIZE_PAIR_VAL(decode)

    DEF_TIME_SIZE_PAIR(scaling)
    DEF_ADD_SIZE_PAIR_VAL(scaling)

#undef DEF_TIME_SIZE_PAIR
#undef DEF_ADD_SIZE_PAIR_VAL

    std::atomic<std::size_t> total_count_image;
    void increment_count(){
        total_count_image += 1;
    }

    std::size_t batch_size;
    std::atomic<std::size_t> batch_count{0};
    std::atomic<double> total_time_batch{0};

    void insert_batch(double time, std::size_t batch_size_){
        total_time_batch.store(total_time_batch.load() + time);
        batch_size = batch_size_;
        batch_count += 1;
    }

    std::atomic<double> total_time_reduce{0};

    void insert_reduce(double time, std::size_t){
        total_time_reduce.store(total_time_reduce.load() + time);
    }

    std::atomic<double> total_io_block{0};

    void insert_io_block(double time){
        total_io_block.store(total_io_block.load() + time);
    }


    std::atomic<double> total_time_schedule{0};

    void insert_schedule(double time, std::size_t){
        total_time_schedule.store(total_time_schedule.load() + time);
    }

    double total_per_thread_time(){
        return total_time_read + total_time_decode + total_time_scaling + total_time_transform;
    }

    void report(double loop, double thread_count = 1, int max_io_thread=1);

private:
    RuntimeStats() = default;
};

#endif
