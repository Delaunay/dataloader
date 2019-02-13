# Dataloader


# Run

    ./io_benchmark --data /imagenet_folder_train/ -n 32 -b 32 -j 2

        --data  imagenet_folder             
        -n      number of batches to fetch  defaults to 32
        -b      batch size                  defaults to 32
        -j      number of threads           defaults to 16
        -io     number of io threads        defaults to 4
        -p      prefetching/buffering size  defaults to 3
        --seed  PRNG seed                   defaults to time(nullptr))
    
After runing it will display the report below.
The sample report was generated using two spindle disks in RAID0 to store the dataset.
We can very clearly see that disk IO was the bottleneck in that case.



         - 32842 images found
         - 100 classes found
         - took 2.07719s to initialize
        ---------------------------------------------------
                            REPORT
        ---------------------------------------------------
                Per Thread     |  Overall
        1. read 1026 images
         -    66.5692      sec |     4.1606      sec
         -    15.4125 file/sec |   246.6006 file/sec
         -     9.2535   Mo/sec |   148.0565   Mo/sec
        2. transform 1026 images
         -    19.7767      sec |     1.2360      sec
         -    51.8792 file/sec |   830.0670 file/sec
         -    31.1983   Mo/sec |   499.1728   Mo/sec
        3. decode 1026 images
         -     9.8698      sec |     0.6169      sec
         -   103.9539 file/sec |  1663.2621 file/sec
         -   311.8616   Mo/sec |  4989.7863   Mo/sec
        4. scaling 1026 images
         -     1.3806      sec |     0.0863      sec
         -   743.1643 file/sec | 11890.6291 file/sec
         -   106.4768   Mo/sec |  1703.6281   Mo/sec
        Total 1026 images
         -    97.5962      sec |    16.6930      sec
         -    10.5127 file/sec |    61.4628 file/sec
         - Overhead -80.9032 sec 
         - Compression Ratio before scaling 4.9921
         - Compression Ratio after scaling 0.2389
         - IO wait 165.3841 sec
        ---------------------------------------------------
        Dataloader 32 batchs 
         -  Sched   0.0042 sec
         -  Batch  16.6606 sec |    61.4625 img/sec
         - Reduce   0.0280 sec | 36557.4010 img/sec
         -  Total  16.6928 sec |    61.3439 img/sec
        ---------------------------------------------------
        Thread Pool Report
           ID       WORK       IDLE   (%)  TASKS  WORK/TASK
            0    16.4763     0.0043  99.97    64     0.2574
            1    16.6905     0.0043  99.97    65     0.2568
            2    16.6916     0.0043  99.97    65     0.2568
            3    16.6141     0.0043  99.97    64     0.2596
            4    16.6100     0.0043  99.97    64     0.2595
            5    16.4774     0.0043  99.97    63     0.2615
            6    16.5331     0.0043  99.97    64     0.2583
            7    16.4817     0.0042  99.97    64     0.2575
            8    16.5182     0.0042  99.97    64     0.2581
            9    16.5331     0.0042  99.97    64     0.2583
           10    16.4014     0.0042  99.97    64     0.2563
           11    16.6132     0.0042  99.97    64     0.2596
           12    16.4809     0.0042  99.97    64     0.2575
           13    16.6858     0.0042  99.97    63     0.2649
           14    16.6135     0.0042  99.97    64     0.2596
           15    16.5208     0.0032  99.98    65     0.2542
        Total   264.9417     0.0668  99.97  1025     0.2585

                         TIME   (%)
        Empty Queue      0.00   0.02
        Full  Queue      0.00   0.00
           All Time     16.70 100.00

            Arrival Rate      7.69
          Departure Rate      7.83


# Compilation

    
    git clone --recurse-submodules -j8 ...
    cd dataloader

    # Get dependencies
    # ================
    # CPU
    mkdir -p dependencies/torch-cpu
    cd dependencies/torch-cpu
    wget https://download.pytorch.org/libtorch/cpu/libtorch-shared-with-deps-latest.zip
    unzip libtorch-shared-with-deps-latest.zip

    # GPU
    mkdir -p dependencies/torch-gpu
    cd dependencies/torch-gpu
    wget https://download.pytorch.org/libtorch/cu90/libtorch-shared-with-deps-latest.zip
    unzip libtorch-shared-with-deps-latest.zip

    # Build
    mkdir build
    cd build
    cmake -DBUILDING_TEST=ON ..
    make

    # Run tests
    make test




