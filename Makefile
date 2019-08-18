
all: test_cpp

tests: test_python test_cpp

test_python:
	pip uninstall -y cpploader
	rm -rf build/lib.linux-x86_64-3.7
	python setup.py install
	python examples/loader.py --data /media/setepenre/UserData/tmp/fake --batch-size 256 --threads 12 --count 10 --mx-io 4

test_cpp:
	(cd build && make)
	./build/bin/io_benchmark --data /media/setepenre/UserData/tmp/fake -n 32 -b 256 -j 12

