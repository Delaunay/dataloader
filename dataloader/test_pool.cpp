
#include "pool.h"

#include <sstream>

typedef float Float;

float sum(std::vector<Float> const* v){
    float s = 0;
    for(auto f : (*v)){
        s += f;
    }
    return s;
}

void ignore(Float){}

int main(int argc, const char* argv[]){
    std::size_t vector_size = 100000000;
    std::size_t queue_size = 100;
    std::size_t worker_size = 10;
    std::size_t request_count = 200;

    for(int i = 0; i < argc; ++i){
        std::string arg = std::string(argv[i]);
        if ("-vs" == arg) {
            std::stringstream ss(argv[i + 1]);
            ss >> vector_size;
        }
        if ("-qs" == arg) {
            std::stringstream ss(argv[i + 1]);
            ss >> queue_size;
        }
        if ("-ws" == arg){
            std::stringstream ss(argv[i + 1]);
            ss >> worker_size;
        }
        if ("-rc" == arg){
            std::stringstream ss(argv[i + 1]);
            ss >> request_count;
        }
        if ("-h" == arg){
            printf("Usage:\n"
                   "    -vs: vector size\n"
                   "    -qs: task queue size\n"
                   "    -ws: worker size\n"
                   "    -rc: request Count\n"
                   "    -h : this help message\n"
                   "\n"
                   "    Compute `rc` sums of `vs` floats using `ws` workers with a task queue size of `qs`\n");
            return 0;
        }
    }

    printf(
        "        Vector size: %lu\n"
        "    Task queue size: %lu\n"
        "      Worker  count: %lu\n"
        "      Request count: %lu\n", vector_size, queue_size, worker_size, request_count
    );

    ThreadPool<std::vector<Float> const*, Float> pool(worker_size, queue_size);

    std::vector<Float> vec(vector_size, 2.0);
    std::vector<std::optional<std::shared_future<Float>>> results;

    for(std::size_t i = 0; i < request_count; ++i){
        while (pool.is_full()){
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
        }

        results.push_back(pool.insert_task(&vec, sum));
    }

    for(auto& val : results){
        if (val.has_value()){
            val.value().wait();
            volatile Float f = val.value().get();
            ignore(f);
        } else{
            // printf("Error ");
        }
    }
    printf("\n");

    pool.shutdown();

    pool.report();

    return 0;
}
