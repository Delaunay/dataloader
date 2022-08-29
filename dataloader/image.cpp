#include "image.h"

#include <cmath>
#include <cstdio>

// Simple proportional scaling
Image &Image::inplace_scale(int nw, int nh)
{
    std::vector<unsigned char> buffer(std::size_t(nw * nh * 3));
    float wratio = float(_width) / float(nw);
    float hratio = float(_height) / float(nh);

    for (int j = 0; j < nh; ++j)
    {
        for (int i = 0; i < nw; ++i)
        {
            float x = j * hratio;
            float y = i * wratio;

            std::size_t pixel = std::size_t(j * (nw * 3) + i * 3);
            std::size_t old_pix1 = std::size_t(std::floor(x) * (_width * 3.f) + std::floor(y) * 3.f);

            buffer[pixel + 0] = _image[old_pix1 + 0]; // * (1 - ix) + _image[old_pix2 + 0] * ix;
            buffer[pixel + 1] = _image[old_pix1 + 1]; // * (1 - ix) + _image[old_pix2 + 1] * ix;
            buffer[pixel + 2] = _image[old_pix1 + 2]; // * (1 - ix) + _image[old_pix2 + 2] * ix;
        }
    }

    _width = nw;
    _height = nh;
    _image = buffer;
    return *this;
}

void Image::save_ppm(const char *name)
{
    FILE *fd = fopen(name, "w");
    fprintf(fd, "P3\n%d %d\n%d\n", width(), height(), 255);

    int k = 0;
    for (std::size_t i = 0; i < _image.size(); i += 3)
    {
        unsigned int r = _image[i + 0], g = _image[i + 1], b = _image[i + 2];
        fprintf(fd, "%d %d %d ", r, g, b);

        k += 1;
        if (k == height())
        {
            fprintf(fd, "\n");
            k = 0;
        }
    }
    fclose(fd);
}

#ifdef TURBO_JPEG
#include <turbojpeg.h>
#endif

void Image::save_jpeg(const char *name, int flag)
{
#ifdef TURBO_JPEG
    tjhandle comp = tjInitCompress();

    std::vector<unsigned char> buffer(_image.size());
    unsigned char *buffers[1];
    buffers[0] = buffer.data();

    std::size_t size = _image.size();

    tjCompress2(comp, _image.data(), width(), 0, height(), TJPF_RGB, buffers, &size, TJSAMP_444, 75, flag);
    tjDestroy(comp);

    FILE *fd = fopen(name, "w");
    fwrite(buffer.data(), sizeof(unsigned char), size, fd);
    fclose(fd);
#endif
}
