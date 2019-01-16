
#include "io.h"
#include "dataset.h"
#include "dataloader.h"
#include "utils.h"

#include <ctime>
#include <sstream>
#include <cstdio>
#include <cstring>

using Path = ImageFolder::Path;

#include <turbojpeg.h>


void save_buffer_ppm(const char* name, std::vector<unsigned char> const& buffer, int w, int h){
    FILE* fd = fopen(name, "w");
    fprintf(fd, "P3\n%d %d\n%d\n", w, h, 255);

    int k = 0;
    for(std::size_t i = 0; i < buffer.size(); i += 3){
        unsigned int r = buffer[i + 0], g = buffer[i + 1], b = buffer[i + 2];
        fprintf(fd, "%d %d %d ", r, g, b);

        k += 1;
        if (k == h){
            fprintf(fd, "\n");
            k = 0;
        }
    }
    fclose(fd);
}

std::vector<unsigned char> scale(std::vector<unsigned char> data, int ow, int oh, int nw, int nh){
    std::vector<unsigned char> buffer(std::size_t(nw * nh * 3));
    float wratio = float(ow) / float(nw);
    float hratio = float(oh) / float(nh);

    for (int j = 0; j < nh; ++j){
        for (int i = 0; i < nw; ++i){
            int x = j * hratio;
            int y = i * wratio;

            std::size_t pixel = std::size_t(j * (nw * 3) + i * 3);
            std::size_t old_pix = std::size_t(x * (ow * 3.f) + y * 3.f);

            buffer[pixel + 0] = data[old_pix + 0];
            buffer[pixel + 1] = data[old_pix + 1];
            buffer[pixel + 2] = data[old_pix + 2];
        }
    }

    return buffer;
}

static double total_scaling_time = 0;
static int total_raw_decode_size = 0;

std::vector<unsigned char> transform(std::vector<unsigned char>& data, int width, int height){
    const int COLOR_COMPONENTS = 3;

    tjtransform xform;
    memset(&xform, 0, sizeof(tjtransform));
    // xform.op = TJXOP_VFLIP;
    xform.op = TJXOP_HFLIP;
    // xform.options |= TJXOPT_GRAY;
    //xform.op = TJXOP_TRANSPOSE;
    // xform.op = TJXOP_TRANSVERSE;
    // xform.op = TJXOP_ROT90;
    // xform.op = TJXOP_ROT180;
    // xform.op = TJXOP_ROT270;
    // xform.options |= TJXOPT_CROP;
    // flags |= TJFLAG_FASTUPSAMPLE;


    // Transform the jpeg directly
    std::vector<unsigned char> transform_buffer(std::size_t(width * height * COLOR_COMPONENTS), 0);

    unsigned char * transform_buffers[1];
    transform_buffers[0] = transform_buffer.data();
    std::size_t dstSize = 0;

    tjhandle tjInstance = tjInitTransform();
    tjTransform(tjInstance, data.data(), data.size(), 1, transform_buffers, &dstSize, &xform, TJFLAG_NOREALLOC);

    tjDestroy(tjInstance);
    return transform_buffer;
}

std::vector<unsigned char> decompress(std::vector<unsigned char> data){
    tjhandle decompressor = tjInitDecompress();

    int jpegSubsamp, width, height;
    tjDecompressHeader2(decompressor, data.data(), data.size(), &width, &height, &jpegSubsamp);

    // Transform
    data = transform(data, width, height);

    // Decode
    std::vector<unsigned char> image(std::size_t(width * height * 3));

    tjDecompress2(decompressor, data.data(), data.size(), image.data(), width, 0, height, TJPF_RGB, TJFLAG_FASTDCT | TJFLAG_NOREALLOC);

    total_raw_decode_size += image.size();

    // Scaling
    int target_width = 256;
    int target_height = 256;

    TimeIt scaling_time;
    data = scale(image, width, height, target_width, target_height);
    total_scaling_time += scaling_time.stop();

    tjDestroy(decompressor);
    return data;
}

int main(int argc, const char* argv[]){

    double total_read_time = 0;
    int count_image = 0;
    int total_read_size = 0;
    int image_to_load = 2048;

    int seed = int(time(nullptr));

    //const char* data_loc = "/home/user1/test_database/imgnet/ImageNet2012_jpeg/train/";
    const char* data_loc = "/media/setepenre/UserData/tmp/jpeg";

    for(int i = 0; i < argc; ++i){
        std::string arg = std::string(argv[i]);
        if ("--data" == arg) {
            data_loc = argv[i + 1];
        }
        if ("-n" == arg) {
            std::stringstream ss(argv[i + 1]);
            ss >> image_to_load;
        }
        if ("--seed" == arg){
            std::stringstream ss(argv[i + 1]);
            ss >> seed;
        }
    }

    double total_decode_time = 0;
    int total_decode_size = 0;

    std::function<int(std::tuple<Path, int, std::size_t> const& item)> bench_fun = [&](std::tuple<Path, int, std::size_t> const& item){
        Path path; int label; std::size_t size;
        std::tie(path, label, size) = item;

        TimeIt read_time;
        std::vector<unsigned char> buffer = load_file(path, size);
        total_read_time += read_time.stop();

        TimeIt decode_time;
        std::vector<unsigned char> image = decompress(buffer);
        total_decode_time += decode_time.stop();

        count_image += 1;
        total_read_size += size;
        total_decode_size += image.size();

        save_buffer_ppm("last.ppm", image, 256, 256);

        return 0;
    };

    try{
        ImageFolder dataset(data_loc, bench_fun);
        DataLoader dataloader(dataset, 1, 1, seed);

        TimeIt loop_time;
        for(int i = 0; i < image_to_load; ++i){
            dataloader.get_next_item();
        }
        double loop = loop_time.stop();

        printf("Read %d images in %.4f sec\n", count_image, total_read_time);
        printf(" - %.4f file/sec\n", count_image / total_read_time);
        printf(" - %.4f Mo/sec\n", (total_read_size / 1024 / 1024) / total_read_time);
        printf(" - %.4f Ko\n", double(total_read_size) / double(count_image) / 1024);

        printf("Decoded %d images in %.4f sec\n", count_image, total_decode_time);
        printf(" - %.4f file/sec\n", count_image / total_decode_time);
        printf(" - %.4f Mo/sec\n", (total_decode_size / 1024 / 1024) / total_decode_time);

        printf("Scaling %d images in %.4f sec\n", count_image, total_scaling_time);
        printf(" - %.4f file/sec\n", count_image / total_scaling_time);
        printf(" - %.4f Mo/sec\n", (3 * 256 * 256 * 256 / 1024 / 1024) / total_scaling_time);

        double total_time = total_decode_time + total_read_time;
        printf("Total %d images in %.4f sec\n", count_image, total_time);
        printf(" - %.4f file/sec\n", count_image / total_time);
        printf(" - Overhead %.4f sec \n", loop - total_read_time - total_decode_time);
        printf(" - Compression Ratio After downscale  %.4f\n", double(total_decode_size) / double(total_read_size));
        printf(" - Compression Ratio before downscale %.4f\n", double(total_raw_decode_size) / double(total_read_size));
    } catch (const std::filesystem::filesystem_error& e){
        printf("%s\n", e.what());
        return -1;
    }

    return 0;
}
