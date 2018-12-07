#if __has_include(<turbojpeg.h>)
#include "jpeg_loader.h"
#include <turbojpeg.h>


#include <iostream>
#include <cstdio>

#define ERROR(msg) std::cout << msg << std::endl


int decompress(std::pair<std::string, int> const data){

}

struct TurboJpegConfiguration{
    TJPF pixel_format = TJPF_RGB;
    TJCS color_space = TJCS_RGB;

};

class TurboJPEGBuffer{
public:
    TurboJPEGBuffer(int bytes){
        _data = tjAlloc(bytes);
    }

    TurboJPEGBuffer(TurboJPEGBuffer&& tmp){
        _data = tmp._data;
        tmp._data = nullptr;
    }

    TurboJPEGBuffer(TurboJPEGBuffer &tmp) = delete;

    ~TurboJPEGBuffer(){
        tjFree(_data);
    }

    unsigned char* data() {
        return _data;
    }

private:
    unsigned char* _data;
};


TurboJPEGBuffer decompress(std::string& jpegBuf,int width,int height){
    //int scaleWidth = TJSCALED(width, tjGetScalingFactors());
    int pitch = 0; //scaledWidth * tjPixelSize[pixelFormat];
    tjhandle _decompress = tjInitCompress();
    TurboJPEGBuffer output = TurboJPEGBuffer(width * height * 3 * sizeof(char));

    tjDecompress2(
        _decompress,
        reinterpret_cast<const unsigned char*>(&jpegBuf[0]),
        jpegBuf.size(),
        output.data(),
        width,
        pitch,
        height,
        TJPF_RGB,
        TJFLAG_NOREALLOC
    );

    tjDestroy(_decompress);
    return output;
}
#endif
