
all: test_cpp

tests: test_python test_cpp

test_python:
	pip uninstall -y cpploader
	rm -rf build/lib.linux-x86_64-3.7
	python setup.py install
	python examples/loader.py --data /media/setepenre/UserData/tmp/train/ --batch-size 256 --threads 12 --count 10 --mx-io 12 --seed 1

test_cpp:
	(cd build && make)
	./build/bin/io_benchmark --data /media/setepenre/UserData/tmp/train/ -n 32 -b 256 -j 12 -io 8

test_cpp2:
	(cd build && make)
	./build/bin/io_benchmark --data /media/setepenre/82C8E73CC8E72CDB/data/ImageNet_ILSVRC2017/ILSVRC2017_CLS-LOC/ILSVRC/Data/CLS-LOC/train/ -n 32 -b 256 -j 12 -io 12

test_zip:
	(cd build && make)
	./build/bin/io_benchmark -zip 1 --data /media/setepenre/UserData/tmp/train.zip -n 32 -b 256 -j 12 -io 4

test_zip2:
	(cd build && make)
	./build/bin/test_zip


