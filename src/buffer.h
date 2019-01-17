#ifndef DATALOADER_BUFFER_HEADER_H
#define DATALOADER_BUFFER_HEADER_H

#include <thread>
#include <mutex>
#include <vector>

template<typename T>
class Option{
public:
    Option():
        empty(true)
    {
        data.b = dummy();
    }

    Option(T const& val):
        empty(false)
    {
        data.a = val;
    }

    bool is_empty() const {    return empty; }

    T const& get() const {    return data.a; }

private:
    struct dummy{};

    union {
        T a;
        dummy b;
    } data;

    bool empty;
};

template<typename T>
Option<T> some(T const& val){
    return Option<T>(val);
}

template<typename T>
Option<T> none(){
    return Option<T>();
}

template<typename T>
class RingBuffer{
public:
    RingBuffer(std::size_t size){
        _buffer.reserve(size);

        for(int i = 0; i < size; ++i){
            _buffer.push_back(0);
        }
    }

    bool push(T const& val){
        std::lock_guard lock(mutex);

        if (size() < _buffer.size()){
            _buffer[end % _buffer.size()] = val;
            end += 1;
            return true;
        }
        return false;
    }

    Option<T> pop(){
        std::lock_guard lock(mutex);

        if (size() > 0){
            auto val = _buffer[start % _buffer.size()];
            start += 1;
            return some(val);
        }
        return none<T>();
    }

    std::size_t size() const {
        return end - start;
    }

    void report(){
        printf("Processed Item: %lu\nInserted Item: %lu\n", start, end);
    }

private:
    std::size_t start = 0;
    std::size_t end = 0;

    std::mutex mutex;
    std::vector<T> _buffer;
};


#endif
