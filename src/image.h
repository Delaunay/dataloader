#ifndef DATALOADER_IMAGE_HEADER_H
#define DATALOADER_IMAGE_HEADER_H

#include <vector>

class Image{
public:
    Image(std::vector<unsigned char> const& img, int w, int h):
        _image(img), _width(w), _height(h)
    {}

    int width()  const { return _width; }
    int height() const { return _height; }
    int size()   const { return int(_image.size());}


    Image& inplace_scale(int nw, int nh);
    void save_ppm(const char* name);

private:
    std::vector<unsigned char> _image;
    int _width = 0;
    int _height = 0;
};

#endif
