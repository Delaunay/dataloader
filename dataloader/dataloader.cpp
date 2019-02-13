#include "dataloader.h"


DataLoader::DataLoader(ImageFolder const& dataset, std::size_t batch_size_, std::size_t worker_cout, std::size_t buffering_, int seed, std::size_t io):
    dataset(dataset), buffering(buffering_), batch_size(batch_size_), seed(seed), pool(worker_cout, batch_size_ * buffering_)
{
    DLOG("%s", "init dataloader");

    if (io == 0){
        io = worker_cout;
    }

    make_io_lock(io);

    image_indices = std::vector<std::size_t>(dataset.size());

    for(std::size_t i = 0; i < std::size_t(dataset.size()); ++i){
        image_indices[i] = i;
    }

    shuffle();

    future_buffered_batch = std::vector<std::vector<std::shared_future<Image>>>(buffering);

    for(std::size_t i = 0; i < buffering; ++i){
       future_buffered_batch[i] = std::vector<std::shared_future<Image>>(batch_size);
    }

    DLOG("%s", "Starting the load image request");
    for(std::size_t i = 1; i < buffering; ++i){
       send_next_batch();
    }
}

std::size_t DataLoader::get_next_image_index(){
    std::size_t i = image_iterator;
    image_iterator += 1;

    // start new epoch
    if (image_iterator >= dataset.size()){
        _epoch += 1;
        image_iterator = 0;
        if (reshuffle_after_epoch){
            shuffle();
        }
    }

    return image_indices[i];
}

void DataLoader::send_next_batch(){
    std::vector<std::shared_future<Image>>& future_batch = future_buffered_batch[buffering_index];

    TimeIt schedule_time;
    for(std::size_t i = 0; i < batch_size;){
        int index = int(get_next_image_index());

        auto val = pool.insert_task(index, [&](int index){
            return dataset.get_item(index);
        });

        if (val.has_value()){
            future_batch[i] = val.value();
            i += 1;
        } else {
            printf("Error image skipped (full: %d, size: %lu)\n", pool.is_full(), pool.size());
        }
    }
    RuntimeStats::stat().insert_schedule(schedule_time.stop(), batch_size);
    buffering_index = (buffering_index + 1) % buffering;
}

std::vector<Image> DataLoader::get_next_item(){
    send_next_batch();
    return reduce_to_vector(get_future_batch());
}

std::vector<std::shared_future<Image>>& DataLoader::get_future_batch(){
    DLOG("%s", "Waiting for images");
    std::vector<std::shared_future<Image>>& future_batch = future_buffered_batch[next_batch];
    next_batch = (next_batch + 1) % buffering;

    TimeIt batch_time;
    for(std::size_t i = 0; i < batch_size; ++i){
        DLOG("%s", "waiting for image (id: %lu)", i);
        std::shared_future<Image>& fut = future_batch[i];
        fut.wait();
    }
    RuntimeStats::stat().insert_batch(batch_time.stop(), batch_size);

    DLOG("%s", "Images ready");
    return future_batch;
}

std::vector<Image> DataLoader::reduce_to_vector(std::vector<std::shared_future<Image>>& future_batch){
    std::vector<Image> batch;
    batch.reserve(batch_size);

    TimeIt reduce_time;
    for(std::size_t i = 0; i < batch_size; ++i){
        std::shared_future<Image>& fut = future_batch[i];
        batch.push_back(std::move(fut.get()));
    }
    RuntimeStats::stat().insert_reduce(reduce_time.stop(), batch_size);

    return batch;
}

void DataLoader::shuffle(){
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
