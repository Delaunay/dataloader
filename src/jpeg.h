#ifndef DATALOADER_JPEG_HEADER_H
#define DATALOADER_JPEG_HEADER_H

#include "image.h"

#include <turbojpeg.h>

class Transform{
public:
    Transform();

    tjtransform* transform() {
        return &xform;
    }

    Transform& hflip      (){   xform.op = TJXOP_HFLIP;       return *this; }
    Transform& vflip      (){   xform.op = TJXOP_VFLIP;       return *this; }
    Transform& transpose  (){   xform.op = TJXOP_TRANSPOSE;   return *this; }
    Transform& transverse (){   xform.op = TJXOP_TRANSVERSE;  return *this; }
    Transform& rot90      (){   xform.op = TJXOP_ROT90;       return *this; }
    Transform& rot180     (){   xform.op = TJXOP_ROT180;      return *this; }
    Transform& rot270     (){   xform.op = TJXOP_ROT270;      return *this; }

private:
    tjtransform xform;
};

class JpegImage{
public:
    JpegImage(const char* name, std::size_t file_size);

    ~JpegImage();

    JpegImage(JpegImage const& img);

    JpegImage& inplace_transform(Transform& xform, int flag = TJFLAG_NOREALLOC);

    Image decode(int flag = TJFLAG_FASTDCT | TJFLAG_NOREALLOC);

    int width()  const { return _width; }
    int height() const { return _height; }
    int size()   const { return int(_image.size());}

private:
    tjhandle _decompressor{tjInitDecompress()};
    std::vector<unsigned char> _image;
    int _width = 0;
    int _height = 0;
    int _jpeg_subsamp = 0;
};

#endif
