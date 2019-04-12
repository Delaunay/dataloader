#include "runtime.h"

void RuntimeStats::report(double loop, double thread_count, int max_io_thread){
    int count_image = int(total_count_image);
    printf("---------------------------------------------------\n");
    printf("                    REPORT\n");
    printf("---------------------------------------------------\n");

    double total_time = total_per_thread_time();

    //if (!per_thread)
    //    total_time = loop;

    printf("        Per Thread     |  Overall\n");

#define REPORT(num, name, thd_count)\
    printf(#num ". " #name " %d images\n", count_image);\
    printf(" - %10.4f      sec | %10.4f      sec\n", total_time_##name.load(), total_time_##name.load() / thd_count);\
    printf(" - %10.4f file/sec | %10.4f file/sec\n", count_image / total_time_##name.load(), count_image / (total_time_##name.load() / thd_count));\
    printf(" - %10.4f   Mo/sec | %10.4f   Mo/sec\n", \
        (total_size_##name.load() / 1024 / 1024) / total_time_##name.load(),\
        (total_size_##name.load() / 1024 / 1024) / (total_time_##name.load() / thd_count));

    REPORT(1, read, max_io_thread);
    REPORT(2, transform, thread_count);
    REPORT(3, decode, thread_count);
    REPORT(4, scaling, thread_count);

#undef REPORT


    printf("Total %d images\n", count_image);
    printf(" - %10.4f      sec | %10.4f      sec\n", total_time, loop);
    printf(" - %10.4f file/sec | %10.4f file/sec\n", count_image / total_time, count_image / loop);
    printf(" - Overhead %.4f sec \n", loop - total_time);
    printf(" - Compression Ratio before scaling %.4f\n", double(total_size_decode.load()) / double(total_size_read.load()));
    printf(" - Compression Ratio after scaling %.4f\n", double(total_size_scaling.load()) / double(total_size_read.load()));
    printf(" - IO wait %.4f sec\n", total_io_block.load());
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
