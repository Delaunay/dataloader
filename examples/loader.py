import torch
import torchvision

import cpploader

import argparse
import time
import sys

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

print('Creating dataset')
folder = cpploader.Dataset('ImageFolder', args.data, True)

print('Creating Sampler')
sampler = cpploader.Sampler('RandomSampler', folder.size(), args.seed)

print('Setting up Loader')
loader = cpploader.Loader(
    folder,
    sampler,
    args.batch_size,
    args.threads,
    args.buffering,
    args.seed,
    args.mx_io
)


model = torchvision.models.resnet18().to(device="cuda")
optimizer = torch.optim.SGD(param_copy, lr = 0.01, momentum = 0.9);


print('Setup')
for i in range(0, 10):
    b, t = loader.next()
    print(i, b.shape, t.shape, b.sum())

all = 0


print('Iterate')

all = time.time()
all_io = 0
all_compute = 0

for i in range(0, args.count):

    # IO
    io = time.time()
    b, t = loader.next()

    inp = b.to_cuda()
    target = t.to_cuda()

    all_io += time.time() - io

    # Compute
    compute = time.time()

    optimizer.zero_grad()
    out = model(inp)
    loss = torch.nn.functional.cross_entropy(out, target)
    value = loss.backward()

    torch.cuda.synchronize()
    all_compute += time.time() - compute

all = time.time() - all


print('        Speed: {}'.format(args.batch_size * args.count / all))
print('     Speed IO: {}'.format(args.batch_size * args.count / all_io))
print('Speed Compute: {}'.format(args.batch_size * args.count / all_compute))


loader.report()
loader.shutdown()
print('done')

sys.exit()



