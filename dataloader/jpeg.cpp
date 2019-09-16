#include "jpeg.h"
#include "io.h"
#include "utils.h"

#include <cstdio>
#include <cstring>

#include <coz.h>

#undef DLOG
#define DLOG(...)

Transform::Transform(){
    memset(&xform, 0, sizeof(tjtransform));
}

JpegImage::JpegImage(const char* name, std::size_t file_size){
    DLOG("Init Jpeg from file");
    _image = load_file(name, file_size);

    tjDecompressHeader2(
        _decompressor,
        _image.data(),
        _image.size(),
        &_width,
        &_height,
        &_jpeg_subsamp
    );

    assert(_width > 0 && _height > 0);
}

JpegImage::JpegImage(const Bytes& data){
    DLOG("Init Jpeg from buffer");
    _image = Bytes(data);

    assert(_image.size() > 0);
    int rc = tjDecompressHeader2(
        _decompressor,
        _image.data(),
        _image.size(),
        &_width,
        &_height,
        &_jpeg_subsamp
    );

    if (rc != 0){
        DLOG("header read (rc: %d) => %s", rc, tjGetErrorStr2(_decompressor));
    }
    assert(_width > 0 && _height > 0);
}


JpegImage::~JpegImage(){
    tjDestroy(_decompressor);
}

JpegImage::JpegImage(JpegImage const& img):
    _image(img._image), _width(img.width()), _height(img.height()),
    _jpeg_subsamp(img._jpeg_subsamp)
{}

JpegImage& JpegImage::inplace_transform(Transform& xform, int flag){
    if (!bool(xform)){
        return *this;
    }

    // Transform the jpeg directly
    std::vector<unsigned char> transform_buffer(_image.size() * 2, 0);

    unsigned char * transform_buffers[1];
    transform_buffers[0] = transform_buffer.data();
    std::size_t dstSize = _image.size() * 2;

    tjhandle tjInstance = tjInitTransform();

    tjTransform(tjInstance, _image.data(), _image.size(), 1, transform_buffers, &dstSize, xform.transform(), flag);

    tjDestroy(tjInstance);

    transform_buffer.resize(dstSize);
    _image = transform_buffer;
    return *this;
}

Image JpegImage::decode(int flag){
    // Decode
    std::vector<unsigned char> buffer(std::size_t(width() * height() * 3));

    DLOG("decoding");
    tjDecompress2(_decompressor, _image.data(), _image.size(), buffer.data(), width(), 0, height(), TJPF_RGB, flag);

    DLOG("decoded");
    COZ_PROGRESS_NAMED("decode")
    return Image(buffer, width(), height());
}

