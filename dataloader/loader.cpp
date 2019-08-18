
#include "loader.h"
#include "jpeg.h"
#include "runtime.h"
#include "pool.h"
#include "io.h"

#include "utils.h"

#undef DLOG
#define DLOG(...)

Image single_threaded_loader(std::tuple<std::string, int, std::size_t> const& item){
    std::string path; int label; std::size_t size;
    std::tie(path, label, size) = item;
    DLOG("Starting task %s %i %lu", path.c_str(), label, size);

    // Read
    DLOG("%s", "Waiting for IO resource");
    TimeIt io_block_time;
    start_io();
    RuntimeStats::stat().insert_io_block(io_block_time.stop());

    DLOG("%s", "Reading bytes", path);
    TimeIt read_time;
    auto jpeg = JpegImage(path.c_str(), size);
    end_io();
    RuntimeStats::stat().insert_read(read_time.stop(), size);

    Transform trans;
    // trans.hflip();

    // Transform
    TimeIt transform_time;
    jpeg.inplace_transform(trans);
    RuntimeStats::stat().insert_transform(transform_time.stop(), jpeg.size());

    DLOG("%s", "Decoding");
    // decode
    TimeIt decode_time;
    Image img = jpeg.decode();
    RuntimeStats::stat().insert_decode(decode_time.stop(), img.size());

    DLOG("%s - %d", "Scaling", int(img(112, 112)));
    // Scale
    TimeIt scale_time;
    img.inplace_scale(224, 224);

    DLOG("%s - %d", "Scaled", int(img(112, 112)));

    RuntimeStats::stat().insert_scaling(scale_time.stop(), img.size());
    RuntimeStats::stat().increment_count();

    return img;
}
