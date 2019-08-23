#include "sampler.h"

void RandomSampler::init_state(int size_, int seed){
    seed             = 0;
    size             = std::max(size, size_);

    prng_engine   = std::mt19937{seed};
    image_indices = std::vector<std::size_t>(size);

    DLOG("Init State (size: %d, seed: %d, vector: %d)", size, seed, int(image_indices.size()));
    for(std::size_t i = 0; i < std::size_t(size); ++i){
        image_indices[i] = i;
    }
}

std::size_t RandomSampler::sample(){
    std::size_t i = image_indices[current_position];
    current_position += 1;

    if (current_position >= size){

        DLOG("New Epoch %d %d", current_position, size);
        _epoch += 1;
        current_position = 0;

        if (reshuffle_after_epoch){
            shuffle();
        }
    }

    return i;
}

void RandomSampler::shuffle(){
    auto rand = [this](std::size_t n) -> std::size_t {
        std::uniform_int_distribution<std::size_t> a(0, n);
        return a(prng_engine);
    };

    std::random_shuffle(
        std::begin(image_indices),
        std::end(image_indices),
        rand
    );
}
