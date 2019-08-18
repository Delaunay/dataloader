#!/bin/bash

rm -rf dependencies/libtorch

cd dependencies/

wget https://download.pytorch.org/libtorch/cpu/libtorch-shared-with-deps-latest.zip

unzip libtorch-shared-with-deps-latest.zip

rm libtorch-shared-with-deps-latest.zip


