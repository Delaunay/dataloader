
#include "loader.h"
#include "jpeg.h"
#include "runtime.h"
#include "pool.h"

#include "utils.h"

#undef DLOG
#define DLOG(...)
#include <coz.h>



Image single_threaded_loader(const Bytes &data){
    DLOG("Starting task");

    JpegImage jpeg(data);

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
    COZ_PROGRESS_NAMED("image_decode");

    DLOG("done");
    RuntimeStats::stat().insert_decode(decode_time.stop(), img.size());

    DLOG("%s - %d", "Scaling", int(img(112, 112)));
    // Scale
    TimeIt scale_time;
    img.inplace_scale(224, 224);
    COZ_PROGRESS_NAMED("image_scale");

    DLOG("%s - %d", "Scaled", int(img(112, 112)));

    RuntimeStats::stat().insert_scaling(scale_time.stop(), img.size());
    RuntimeStats::stat().increment_count();

    COZ_PROGRESS_NAMED("image_loader");
    return img;
}
