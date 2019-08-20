#include "dataloader.h"

#include <cstring>
#include <iostream>
#include <thread>

DataLoader::DataLoader(const Dataset& dataset, int batch_size_, Transform trans, int worker_cout, int buffering_, int seed, int io):
    dataset(dataset), buffering(buffering_), batch_size(batch_size_),
    workers(worker_cout), io_threads(io), seed(seed), trans(trans), pool(worker_cout, batch_size_ * buffering_)
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

    memory_pool = std::vector<uint8_t>(batch_size_ * image_size() * buffering_);
    image_ready = std::vector<int>(batch_size_ * buffering_, -1);

    for(int i = 0; i < buffering_; ++i){
        DLOG("%s", "send batch request");
        send_next_batch();
    }
}

std::tuple<std::vector<uint8_t>, std::vector<int> > DataLoader::get_next_item(){
    DLOG("getting batch");
    auto ret = get_future_batch();

    DLOG("sending work");
    send_next_batch();
    return ret;
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
    DLOG("> Send next batch (retrieve_batch_id: %d) (sending_batch_id: %d)", retrieve_batch, sent_batch);

    TimeIt schedule_time;
    assert(trans != nullptr);

    for(int i = 0; i < batch_size;){
        int index = int(get_next_image_index());
        int img_index = sent_batch * batch_size + i;

        DLOG(">> Before Schedule %d = %d * %d", img_index, i, sent_batch);

        auto val = pool.insert_task(std::make_tuple(index, img_index), [&](std::tuple<int, int> input) -> bool {
            int index, img_index;
            std::tie(index, img_index) = input;

            MappedStorage<uint8_t> mem = image_mem(img_index);

            // read image
            std::tuple<Bytes, int> sample = dataset.get_item(index);

            // Transform Image
            const Bytes& data = std::get<0>(sample);
            Image img = trans(data);

            // Copy to storage
            assert(mem.size() == img.size());
            memcpy(mem.data(), img.data(), mem.size());

            // Image is ready to be consumed
            mark_ready(img_index, std::get<1>(sample));
            DLOG(">> Image (id: %d) is ready", img_index);
            return true;
        });

        if (val.has_value()){
            i += 1;
        } else {
            printf("Error image skipped (full: %d, size: %d)\n", pool.is_full(), pool.size());
        }
    }

    RuntimeStats::stat().insert_schedule(schedule_time.stop(), batch_size);
    sent_batch = (sent_batch + 1) % buffering;

    DLOG("< Done sending work (sending_batch_id: %d)", sent_batch);
}

std::tuple<std::vector<uint8_t>, std::vector<int>> DataLoader::get_future_batch(){
    // copy out of the dataloader so the memory can be reused
    // The user becomes now the owner of the data
    DLOG("> Waiting for images (retrieve_batch_id: %d) (sending_batch_id: %d)", retrieve_batch, sent_batch);

    std::vector<uint8_t> result(batch_size * image_size());
    std::vector<int> labels(batch_size);

    TimeIt batch_time;
    int img_offset = 0;
    int max_id = batch_size * buffering;

    for(int i = 0; i < batch_size;){
        int img_idx = batch_size * retrieve_batch + img_offset;
        DLOG(">> Waiting for (id: %d) = %d * %d + %d  (max: %d)",
             img_idx, batch_size, retrieve_batch, img_offset, max_id);

        MappedStorage<uint8_t> mem = image_mem(img_idx);

        // wait for the image to be ready
        int wait_time = 0;
        while(!is_ready(img_idx)){
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            wait_time += 1;

            if (wait_time > 10000){
                break;
            }
        }

        if (is_ready(img_idx)){
           memcpy(result.data() + i * image_size(), mem.data(), mem.size());
           labels[i] = image_ready[img_idx];
           i += 1;
        } else {
           ELOG(">> Skipping img %d waited too long (batch_size: %d)", img_idx, i);
        }

        img_offset = (img_offset + 1) % batch_size;
        DLOG("> Mark empty (id: %d)", img_idx);
        mark_empty(img_idx);
    }

    retrieve_batch = (retrieve_batch + 1) % buffering;
    RuntimeStats::stat().insert_batch(batch_time.stop(), batch_size);

    DLOG("< Batch Ready (retrieve_batch_id: %d) (%d)", retrieve_batch);
    return std::make_tuple(result, labels);
}

void DataLoader::shuffle(){
    DLOG("shuffle");

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
