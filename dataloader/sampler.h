#ifndef DATALOADER_SAMPLER_HEADER_H
#define DATALOADER_SAMPLER_HEADER_H

#include <random>
#include <functional>
#include <algorithm>

#include "utils.h"

//#undef DLOG
//#define DLOG(...)

class AbstractSampler{
  public:
    virtual void        init_state      (int size, int seed=0) = 0;
    virtual std::size_t sample          () = 0;
    virtual void        shuffle         () = 0;
    virtual int         sampled_count   () const = 0;
    virtual int         epoch           () const = 0;

    virtual ~AbstractSampler() = default;
};


// Public API
class Sampler{
public:
    Sampler(std::string const& sampler, int size, int seed);

    void init_state(int size, int seed=0){
        DLOG("Init State (size: %d, seed: %d", size, seed);
        return _sampler->init_state(size, seed);
    }

    std::size_t sample(){
        return _sampler->sample();
    }

    void shuffle(){
        return _sampler->shuffle();
    }

    int sampled_count() const {
        return _sampler->sampled_count();
    }

    int epoch() const {
        return _sampler->epoch();
    }

private:
    template<typename T>
    static std::shared_ptr<AbstractSampler> make(){
        return std::make_shared<T>();
    }

    static Dict<std::string, std::shared_ptr<AbstractSampler>(*)()> init_factories();

    static Dict<std::string, std::shared_ptr<AbstractSampler>(*)()>& factories(){
        static Dict<std::string, std::shared_ptr<AbstractSampler>(*)()> _factories = init_factories();
        return _factories;
    }

public:
    //! Register a custom backend
    template<typename T>
    static void register_sampler(std::string const& name){
        factories()[name] = make<T>;
    }

    //! return all the names of the registered backend
    std::vector<std::string> registered_backend(){
        std::vector<std::string> keys;
        keys.reserve(factories().size());

        for (auto& val: factories()){
            keys.push_back(val.first);
        }
        return keys;
    }

private:
    std::shared_ptr<AbstractSampler> _sampler = nullptr;
};


class RandomSampler: public AbstractSampler{
public:
    RandomSampler(int size=0, int seed=0){
        init_state(size, seed);
    }

    //! Initialize Sampler
    void init_state(int size, int seed=0) override;

    //! Return the item/image index we should put in our batch next
    std::size_t sample() override;

    //! Shuffle the list of indices
    void shuffle() override;

    //! Number of images that were drawn in that epoch to date
    int sampled_count() const override {   return current_position;}

    //! Number of times we went through the entire dataset
    int epoch() const         override {   return _epoch; }

public:
    int  seed;
    int  size;
    bool reshuffle_after_epoch = true;

private:
    int                      _epoch           = 0;
    int                      current_position = 0;
    std::mt19937             prng_engine{seed};
    std::vector<std::size_t> image_indices;
};

class SequentialSampler: public AbstractSampler{
public:
    SequentialSampler(int size=0, int seed=0){
        init_state(size, seed);
    }

    //! Initialize Sampler
    void init_state(int size, int seed=0) override {
        size = size;
    }

    //! Return the item/image index we should put in our batch next
    std::size_t sample() override {
        std::size_t i = current_position;
        current_position += 1;

        if (current_position >= size){
            _epoch += 1;
            current_position = 0;
            if (reshuffle_after_epoch){
                shuffle();
            }
        }

        return i;
    }

    //! Shuffle the list of indices
    void shuffle() override {}

    //! Number of images that were drawn in that epoch to date
    int sampled_count() const override {   return current_position;}

    //! Number of times we went through the entire dataset
    int epoch() const         override {   return _epoch; }

public:
    int  seed;
    int  size;
    bool reshuffle_after_epoch = true;

private:
    int                      _epoch           = 0;
    int                      current_position = 0;
};

#endif
