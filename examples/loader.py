import sys
import torch
import cpploader
import argparse
import time

parser = argparse.ArgumentParser()
parser.add_argument('--data', type=str)
parser.add_argument('--batch-size', type=int, default=128)
parser.add_argument('--threads', type=int, default=16)
parser.add_argument('--mx-io', type=int, default=4)
parser.add_argument('--buffering', type=int, default=4)
parser.add_argument('--seed', type=int, default=0)
parser.add_argument('--count', type=int, default=100)
args = parser.parse_args()

print('Setting up Image Folder')
print(args.data)
folder = cpploader.ImageFolder(args.data, True)

print('Setting up  Loader')
loader = cpploader.Loader(
    folder,
    args.batch_size,
    args.threads,
    args.buffering,
    args.seed,
    args.mx_io
)

print('Setup')
for i in range(0, 10):
    b = loader.next()
    print(i, b.shape, b.sum())

all = 0

print('Iterate')
s = time.time()
for i in range(0, args.count):
    print('Starting', i)
    b = loader.next()
    print(b[0, 0, 0, 0])

e = time.time()

print('Speed: {}'.format(args.batch_size * args.count / (e - s)))

loader.report()
loader.shutdown()
print('done')

sys.exit()



