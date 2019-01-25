
#include "loader.h"

#include "dataloader.h"
#include "jpeg.h"
#include "utils.h"
#include "runtime.h"
#include "pool.h"
#include "io.h"

#undef DLOG
#define DLOG(...)

Image single_threaded_loader(std::tuple<std::filesystem::path, int, std::size_t> const& item){
    ImageFolder::Path path; int label; std::size_t size;
    std::tie(path, label, size) = item;

    // Read
    DLOG("Waiting for IO resource");
    TimeIt io_block_time;
    start_io();
    RuntimeStats::stat().insert_io_block(io_block_time.stop());

    DLOG("Reading bytes");
    TimeIt read_time;
    auto jpeg = JpegImage(path.c_str(), size);
    end_io();
    RuntimeStats::stat().insert_read(read_time.stop(), size);

    Transform trans;
    //trans.hflip();

    // Transform
    TimeIt transform_time;
    jpeg.inplace_transform(trans);
    RuntimeStats::stat().insert_transform(transform_time.stop(), jpeg.size());

    DLOG("Decoding");
    // decode
    TimeIt decode_time;
    Image img = jpeg.decode();
    RuntimeStats::stat().insert_decode(decode_time.stop(), img.size());

    DLOG("Scaling");
    // Scale
    TimeIt scale_time;
    img.inplace_scale(224, 224);
    RuntimeStats::stat().insert_scaling(scale_time.stop(), img.size());

    RuntimeStats::stat().increment_count();

    return img;
}
