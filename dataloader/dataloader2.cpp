#include "dataloader2.h"
#include <cstring>

std::vector<uint8_t> DataLoader2::get_next_item(){
    auto ret = get_future_batch();
    send_next_batch();
    return ret;
}


std::size_t DataLoader2::get_next_image_index(){
    //DLOG("get_next_image_index");

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

void DataLoader2::send_next_batch(){
    DLOG("> Send next batch (retrieve_batch_id: %d) (sending_batch_id: %d)", retrieve_batch, sent_batch);

    TimeIt schedule_time;

    for(int i = 0; i < batch_size;){
        int index = int(get_next_image_index());
        int img_index = sent_batch * batch_size + i;

        DLOG(">> Before Schedule %d = %d * %d", img_index, i, sent_batch);

        auto val = pool.insert_task(std::make_tuple(index, img_index), [&](std::tuple<int, int> input) -> bool {
            int index, img_index;
            std::tie(index, img_index) = input;

            MappedStorage<uint8_t> mem = image_mem(img_index);
            // read image
            Image im = dataset.get_item(index);
            // Copy to storage
            memcpy(mem.data(), im.data(), mem.size());
            // Image is ready to be consumed
            mark_ready(img_index);
            DLOG(">> Image (id: %d) is ready", img_index);
            return true;
        });

        if (val.has_value()){
            i += 1;
        } else {
            printf("Error image skipped (full: %d, size: %lu)\n", pool.is_full(), pool.size());
        }
    }

    RuntimeStats::stat().insert_schedule(schedule_time.stop(), batch_size);
    sent_batch = (sent_batch + 1) % buffering;

    DLOG("< Done sending work (sending_batch_id: %d)", sent_batch);
}

std::vector<uint8_t> DataLoader2::get_future_batch(){
    // copy out of the dataloader so the memory can be reused
    // The user becomes now the owner of the data
    DLOG("> Waiting for images (retrieve_batch_id: %d) (sending_batch_id: %d)", retrieve_batch, sent_batch);

    std::vector<uint8_t> result(batch_size * image_size());

    TimeIt batch_time;
    int img_offset = 0;

    for(int i = 0; i < batch_size;){
       int img_idx = batch_size * retrieve_batch + img_offset;
       DLOG(">> Waiting for (id: %d) = %d * %d + %d  (max: %d)", img_idx, batch_size, retrieve_batch, img_offset, batch_size * buffering);

       MappedStorage<uint8_t> mem = image_mem(img_idx);

       // wait for the image to be ready
       while(!is_ready(img_idx)){}

       if (is_ready(img_idx)){
           memcpy(mem.data(), result.data(), mem.size());
           i += 1;
       } else {
           ELOG(">> Skipping img %d waited too long", img_idx);
       }

       img_offset += 1;
       mark_empty(img_idx);
    }

    retrieve_batch = (retrieve_batch + 1) % buffering;
    RuntimeStats::stat().insert_batch(batch_time.stop(), batch_size);

    DLOG("< Batch Ready (retrieve_batch_id: %d) (%d)", retrieve_batch);
    return result;
}

void DataLoader2::shuffle(){
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