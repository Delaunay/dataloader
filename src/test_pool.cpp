
#include "pool.h"


float sum(std::vector<float> const* v){
    float s = 0;
    for(auto f : (*v)){
        s += f;
    }
    return s;
}

int main(){
    ThreadPool<std::vector<float> const*, float> pool(12, 100);

    std::vector<float> vec(100000, 2.0);
    std::vector<std::optional<std::future<float>>> results;

    for(int i = 0; i < 200; ++i){
        while (pool.is_full()){
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }

        results.push_back(pool.insert_task(&vec, sum));
    }

    for(auto& val : results){
        if (val.has_value()){
            val.value().wait();
            float f = val.value().get();
            printf("%.4f ", f);
        } else{
            printf("Error ");
        }
    }
    printf("\n");

    pool.shutdown();

    pool.report();

    return 0;
}
