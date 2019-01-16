#include "jpeg.h"
#include "io.h"

#include <cstdio>
#include <cstring>

Transform::Transform(){
    memset(&xform, 0, sizeof(tjtransform));
}

JpegImage::JpegImage(const char* name, std::size_t file_size){
    _image = load_file(name, file_size);

    tjDecompressHeader2(
        _decompressor,
        _image.data(),
        _image.size(),
        &_width,
        &_height,
        &_jpeg_subsamp
    );
}

JpegImage::~JpegImage(){
    tjDestroy(_decompressor);
}

JpegImage::JpegImage(JpegImage const& img):
    _image(img._image), _width(img.width()), _height(img.height()),
    _jpeg_subsamp(img._jpeg_subsamp)
{}

JpegImage& JpegImage::inplace_transform(Transform& xform, int flag){
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

    tjDecompress2(_decompressor, _image.data(), _image.size(), buffer.data(), width(), 0, height(), TJPF_RGB, flag);

    return Image(buffer, width(), height());
}

