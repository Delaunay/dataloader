#ifndef DATALOADER_JPEG_HEADER_H
#define DATALOADER_JPEG_HEADER_H

#include "image.h"
#include "utils.h"

#include <turbojpeg.h>

class Transform{
public:
    Transform();

    tjtransform* transform() {
        return &xform;
    }

    Transform& hflip      (){   xform.op = TJXOP_HFLIP;       transform_defined = true; return *this; }
    Transform& vflip      (){   xform.op = TJXOP_VFLIP;       transform_defined = true; return *this; }
    Transform& transpose  (){   xform.op = TJXOP_TRANSPOSE;   transform_defined = true; return *this; }
    Transform& transverse (){   xform.op = TJXOP_TRANSVERSE;  transform_defined = true; return *this; }
    Transform& rot90      (){   xform.op = TJXOP_ROT90;       transform_defined = true; return *this; }
    Transform& rot180     (){   xform.op = TJXOP_ROT180;      transform_defined = true; return *this; }
    Transform& rot270     (){   xform.op = TJXOP_ROT270;      transform_defined = true; return *this; }

private:
    tjtransform xform;
    bool transform_defined = false;

public:

    operator bool(){
        return transform_defined;
    }
};

class JpegImage{
public:
    JpegImage(const char* name, std::size_t file_size);

    ~JpegImage();

    JpegImage(JpegImage const& img);

    JpegImage(Bytes const& img);

    JpegImage& inplace_transform(Transform& xform, int flag = TJFLAG_NOREALLOC);

    Image decode(int flag = TJFLAG_FASTDCT | TJFLAG_NOREALLOC);

    int width()  const { return _width; }
    int height() const { return _height; }
    int size()   const { return int(_image.size());}

private:
    tjhandle _decompressor{tjInitDecompress()};
    Bytes _image;
    int _width = 0;
    int _height = 0;
    int _jpeg_subsamp = 0;
};

#endif
