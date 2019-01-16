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

    double total_time(){
        return total_time_read + total_time_decode + total_time_scaling + total_time_transform;
    }

    void report(double loop){
        int count_image = int(total_count_image);

#define REPORT(name)\
        printf(#name " %d images in %.4f sec\n", count_image, total_time_##name.load());\
        printf(" - %.4f file/sec\n", count_image / total_time_##name.load());\
        printf(" - %.4f Mo/sec\n", (total_size_##name.load() / 1024 / 1024) / total_time_##name.load());

        REPORT(read);
        REPORT(transform);
        REPORT(decode);
        REPORT(scaling);

#undef REPORT

        printf("Total %d images in %.4f sec\n", count_image, total_time());
        printf(" - %.4f file/sec\n", count_image / total_time());

        printf(" - Overhead %.4f sec \n", loop - total_time());
        printf(" - Compression Ratio before scaling %.4f\n", double(total_size_decode.load()) / double(total_size_read.load()));
        printf(" - Compression Ratio after scaling %.4f\n", double(total_size_scaling.load()) / double(total_size_read.load()));
    }

private:
    RuntimeStats() = default;
};

#endif
