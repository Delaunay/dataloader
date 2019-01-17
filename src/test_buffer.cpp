#include <cstdio>

#include <future>

#include "buffer.h"



void test(RingBuffer<int>& buf){
    for(int i = 0; i < 100; ++i){
        buf.push(i);

        Option<int> const& val = buf.pop();

        if (!val.is_empty()){
            printf("%d ", val.get());
        }
    }

    printf("\n");

    for(int i = 0; i < 100; ++i){
        if (buf.push(i)){
            printf("%d ", i);
        } else {
            printf("f ");
        }
    }

    printf("\n");

    buf.report();

    for(int i = 0; i < 100; ++i){
        Option<int> const& val = buf.pop();

        if (!val.is_empty()){
            printf("%d ", val.get());
        } else {
            printf("f ");
        }
    }

    printf("\n");
    buf.report();
}

void insert(RingBuffer<int>& buf){
    for(int i = 0; i < 400; ++i){
        if (!buf.push(i)){
            printf("ff ");
        } else {
            printf("s ");
        }
    }
}

void remove(RingBuffer<int>& buf){
    for(int i = 0; i < 400; ++i){
        Option<int> const& val = buf.pop();

        if (!val.is_empty()){
            printf("%d ", val.get());
        } else {
            printf("f ");
        }
    }
}

int main(){
    //*
    RingBuffer<int> buf(10);
    std::vector<std::future<void>> data(2000);

    for (int k = 0; k < 2000; ++k){
        test(buf);
        data[k] = std::async([&](){ return test(buf); });
    }

    for (int k = 0; k < 2000; ++k){
        data[k].wait();
    }
    //*/

    RingBuffer<int> buf2(50);

    printf("\n------------------\n");
    std::future<void> inserter  = std::async([&](){ return insert(buf2); });
    std::future<void> remover   = std::async([&](){ return remove(buf2); });
    std::future<void> inserter1 = std::async([&](){ return insert(buf2); });
    std::future<void> remover1  = std::async([&](){ return remove(buf2); });
    std::future<void> inserter2 = std::async([&](){ return insert(buf2); });
    std::future<void> remover2  = std::async([&](){ return remove(buf2); });
    std::future<void> inserter3 = std::async([&](){ return insert(buf2); });
    std::future<void> remover3  = std::async([&](){ return remove(buf2); });

    remover.wait();
    inserter.wait();
    remover1.wait();
    inserter1.wait();
    remover2.wait();
    inserter2.wait();
    remover3.wait();
    inserter3.wait();
    printf("\n------------------\n\n");

    buf2.report();

    printf("\n");

    return 0;
}
