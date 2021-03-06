
all: test_cpp

tests: test_python test_cpp

compile_python:
	pip uninstall -y cpploader
	rm -rf build/lib.linux-x86_64-3.7
	python setup.py install --user

test_python: compile_python
	python examples/loader.py --data /media/setepenre/UserData/tmp/train/ --batch-size 256 --threads 12 --count 10 --mx-io 12 --seed 1

compile_cpp:
	(cd build && make)

test_cpp: compile_cpp
	./build/bin/io_benchmark --data /media/setepenre/UserData/tmp/train/ -n 32 -b 256 -j 12 -io 8 --seed 12

test_cpp2: compile_cpp
	./build/bin/io_benchmark --data /media/setepenre/82C8E73CC8E72CDB/data/ImageNet_ILSVRC2017/ILSVRC2017_CLS-LOC/ILSVRC/Data/CLS-LOC/train/ -n 32 -b 256 -j 12 -io 12

test_zip: compile_cpp
	./build/bin/io_benchmark -zip 1 --data /media/setepenre/UserData/tmp/train.zip -n 32 -b 256 -j 12 -io 6

test_zip2: compile_cpp
	./build/bin/test_zip

test_pytorch:
	python examples/torch_loader.py --data /media/setepenre/UserData/tmp/train/ --batch-size 128 --threads 12 --count 24 --mx-io 8 --seed 1

test_pytorch_cpp:
	python examples/loader.py --data /media/setepenre/UserData/tmp/train/ --batch-size 128 --threads 12 --count 24 --mx-io 8 --seed 1

test_pytorch_cpp_zip:
	python examples/loader.py --data /media/setepenre/UserData/tmp/train.zip --batch-size 128 --threads 12 --count 24 --mx-io 8 --seed 1 --zip


test_hdd_sequential_zip:
	./build/bin/io_benchmark -zip 1 --data /media/setepenre/UserData/tmp/train.zip -n 32 -b 256 -j 12 -io 6 --seq 1


test_pytorch_nvme:
	python examples/torch_loader.py --data /fast/train/ --batch-size 128 --threads 12 --count 24 --mx-io 8 --seed 1

test_pytorch_cpp_nvme:
	python examples/loader.py --data /fast/train/ --batch-size 128 --threads 12 --count 24 --mx-io 8 --seed 1

test_pytorch_cpp_nvme_zip:
	python examples/loader.py --data /fast/train.zip --batch-size 128 --threads 12 --count 24 --mx-io 8 --seed 1 --zip

test_zip_nvme: compile_cpp
	./build/bin/io_benchmark -zip 1 --data /fast/train.zip -n 32 -b 256 -j 12 -io 8

test_cpp_nvme: compile_cpp
	./build/bin/io_benchmark --data /fast/train/ -n 32 -b 256 -j 12 -io 8 --seed 12


