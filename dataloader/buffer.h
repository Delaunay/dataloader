#ifndef DATALOADER_BUFFER_HEADER_H
#define DATALOADER_BUFFER_HEADER_H

#include <functional>
#include <thread>
#include <mutex>
#include <vector>
#include <variant>

template<typename T>
class RingBuffer{
public:
    //using value_type = std::<T>;
    RingBuffer(int size, std::function<T()> init = [](){ return T(); }){
        _buffer.reserve(size);

        for(int i = 0; i < size; ++i){
            _buffer.push_back(init());
        }
    }

    template<typename... Args>
    T emplace_back(Args&&... args){
        std::lock_guard lock(mutex);

        if (size() < buffer_size()){
            _buffer[end % buffer_size()] = T(std::forward<Args>(args)...);
            T val = _buffer[end % buffer_size()];
            end += 1;
            return val;
        }
        return nullptr;
    }

    bool push(T const& val){
        std::lock_guard lock(mutex);

        if (size() < buffer_size()){
            _buffer[end % buffer_size()] = val;
            end += 1;
            return true;
        }
        return false;
    }

    T pop(){
        std::lock_guard lock(mutex);

        if (size() > 0){
            T val = _buffer[start % buffer_size()];
            start += 1;
            return val;
        }
        return T();
    }

    int buffer_size() const {
        return int(_buffer.size());
    }

    int size() const {
        return end - start;
    }

    bool full() const {
        return size() == buffer_size();
    }

    void report(){
        printf("Processed Item: %lu\nInserted Item: %lu\n", start, end);
    }

private:
    int start = 0;
    int end = 0;

    std::mutex mutex;
    std::vector<T> _buffer;
};


#endif
