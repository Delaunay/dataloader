import torch
import cpploader
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('--data', type=str)


args = parser.parse_args()

print('Setting up Image Folder')
print(args.data)
folder = cpploader.ImageFolder(args.data, True)

print('Setting up  Loader')
loader = cpploader.Loader(
    folder,
    128,
    16,
    2
)

for b in loader:
    pass


loader.report()


