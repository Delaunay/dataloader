#ifndef DATALOADER_IMAGE_HEADER_H
#define DATALOADER_IMAGE_HEADER_H

#include <vector>
#include <turbojpeg.h>

class Image{
public:
    Image(std::vector<unsigned char> const& img, int w, int h):
        _image(img), _width(w), _height(h)
    {}

    int width()  const { return _width; }
    int height() const { return _height; }
    int size()   const { return int(_image.size());}

    Image(Image&& img):
        _image(std::move(img._image)), _width(img.width()), _height(img.height())
    {}

    Image(Image const& img):
        _image(img._image), _width(img.width()), _height(img.height())
    {}

    Image& inplace_scale(int nw, int nh);

    void save_ppm(const char* name);

    void save_jpeg(const char* name, int flag = TJFLAG_FASTDCT);

    unsigned char const * data() const {
        return _image.data();
    }

private:
    std::vector<unsigned char> _image;
    int _width = 0;
    int _height = 0;
};

#endif
