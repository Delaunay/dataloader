from setuptools import setup
from torch.utils.cpp_extension import CppExtension, BuildExtension

import glob

excluded_files = {'io_benchmark.cpp', 'main.cpp'}

def is_wanted(file_name):
    wanted = 'test' not in file_name

    if wanted:
        for excluded in excluded_files:
            if excluded in file_name:
                return False

    return wanted


files = glob.glob('dataloader/*.cpp')
files = list(filter(is_wanted, files))

for f in files:
    print(f)

setup(
    name='cpploader',
    ext_modules=[
        CppExtension(
            name='cpploader', 
            sources=files,
            extra_compile_args=['-std=c++17'],
            libraries = [
                'boost_system', 
                'boost_filesystem',
                'turbojpeg'
            ]
        )
    ],
    cmdclass={
        'build_ext': BuildExtension
    }
)
