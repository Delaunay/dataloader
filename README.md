# Dataloader


# Run

    ./io_benchmark --data /imagenet_folder/ -n 32 -b 32 -j 2

        --data  imagenet_folder             
        -n      number of batches to fetch  defaults to 32
        -b      batch size                  defaults to 32
        -j      number of threads           defaults to 16
        -io     number of io threads        defaults to 4
        -p      prefetching/buffering size  defaults to 3
        --seed  PRNG seed                   defaults to time(nullptr))
    

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




