

Initial Test

* Single Threaded
* 2 Hard Drives in RAID0
    
    work/dataloader/build
    ▶ ./bin/io_benchmark --data /media/setepenre/UserData/tmp/fake -n 2048 
     - 32842 images found
     - 100 classes found
     - took 2.22461s to initialize
    Read 2048 images in 20.6777 sec
     - 99.0439 file/sec
     - 59.5327 Mo/sec
     - 615.9674 Ko
    Decoded 2048 images in 26.3394 sec
     - 77.7543 file/sec
     - 14.5789 Mo/sec
    Scaling 2048 images in 3.7789 sec
     - 541.9519 file/sec
     - 12.7020 Mo/sec
    Total 2048 images in 47.0171 sec
     - 43.5586 file/sec
     - Overhead 0.0198 sec 
     - Compression Ratio After downscale  0.3117
     - Compression Ratio before downscale -1.662


