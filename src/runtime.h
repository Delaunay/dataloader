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
    }

    std::atomic<double> total_time_reduce{0};

    void insert_reduce(double time, std::size_t){
        batch_count += 1;
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

    void report(double loop, double thread_count = 1){
        int count_image = int(total_count_image);
        printf("---------------------------------------------------\n");
        printf("                    REPORT\n");
        printf("---------------------------------------------------\n");

        double total_time = total_per_thread_time();

        //if (!per_thread)
        //    total_time = loop;

        printf("        Per Thread     |  Overall\n");

#define REPORT(num, name)\
        printf(#num ". " #name " %d images\n", count_image);\
        printf(" - %10.4f      sec | %10.4f      sec\n", total_time_##name.load(), total_time_##name.load() / thread_count);\
        printf(" - %10.4f file/sec | %10.4f file/sec\n", count_image / total_time_##name.load(), count_image / (total_time_##name.load() / thread_count));\
        printf(" - %10.4f   Mo/sec | %10.4f   Mo/sec\n", \
            (total_size_##name.load() / 1024 / 1024) / total_time_##name.load(),\
            (total_size_##name.load() / 1024 / 1024) / (total_time_##name.load() / thread_count));

        REPORT(1, read);
        REPORT(2, transform);
        REPORT(3, decode);
        REPORT(4, scaling);

#undef REPORT


        printf("Total %d images\n", count_image);
        printf(" - %10.4f      sec | %10.4f      sec\n", total_time, loop);
        printf(" - %10.4f file/sec | %10.4f file/sec\n", count_image / total_time, count_image / loop);
        printf(" - Overhead %.4f sec \n", loop - total_time);
        printf(" - Compression Ratio before scaling %.4f\n", double(total_size_decode.load()) / double(total_size_read.load()));
        printf(" - Compression Ratio after scaling %.4f\n", double(total_size_scaling.load()) / double(total_size_read.load()));

        printf("---------------------------------------------------\n");


        printf("Dataloader %lu batchs \n", batch_count.load());
        double process_time = (total_time_reduce.load()  + total_time_batch.load() + total_time_schedule.load());
        double batch_img  = double(batch_size) / (total_time_batch.load() / double(batch_count));
        double reduce_img = double(batch_size) / (total_time_reduce.load() / double(batch_count));
        double total_img  = double(batch_size) / (process_time / double(batch_count));

        printf(" -  Sched %8.4f sec\n", total_time_schedule.load());
        printf(" -  Batch %8.4f sec | %10.4f img/sec\n", total_time_batch.load() , batch_img);
        printf(" - Reduce %8.4f sec | %10.4f img/sec\n", total_time_reduce.load() , reduce_img);
        printf(" -  Total %8.4f sec | %10.4f img/sec\n", process_time, total_img);
        printf("---------------------------------------------------\n");

    }

private:
    RuntimeStats() = default;
};

#endif
