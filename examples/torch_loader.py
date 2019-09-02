import torch
import torchvision

import cpploader

import argparse
import time
import sys
import os

parser = argparse.ArgumentParser()
parser.add_argument('--data', type=str)
parser.add_argument('--batch-size', type=int, default=128)
parser.add_argument('--threads', type=int, default=16)
parser.add_argument('--mx-io', type=int, default=4)
parser.add_argument('--buffering', type=int, default=4)
parser.add_argument('--seed', type=int, default=0)
parser.add_argument('--count', type=int, default=100)
parser.add_argument('--no-compute', action='store_true', default=False)
args = parser.parse_args()

print('Setting up Image Folder')
print(args.data)

print('Setting up Loader')
normalize = torchvision.transforms.Normalize(
    mean=[0.485, 0.456, 0.406],
    std=[0.229, 0.224, 0.225]
)

data_transforms = torchvision.transforms.Compose([
    torchvision.transforms.RandomResizedCrop(224),
    torchvision.transforms.RandomHorizontalFlip(),
    torchvision.transforms.ToTensor(),
    normalize
])

dataset = torchvision.datasets.ImageFolder(
    args.data,
    transform=data_transforms
)

dataloader = torch.utils.data.DataLoader(
    dataset,
    batch_size=args.batch_size,
    shuffle=True,
    num_workers=args.threads,
    pin_memory=True
)
loader = iter(dataloader)

model = torchvision.models.resnet18().to(device="cuda")
optimizer = torch.optim.SGD(model.parameters(), lr = 0.01, momentum = 0.9);

print('Setup', len(dataloader))
for i in range(0, 14):
    b, t = next(loader)
    print(i, b.shape, t.shape, b.sum())

all = 0


print('Iterate')

all = time.time()
all_io = 0
all_compute = 0

for i in range(0, args.count):

    # IO
    io = time.time()
    b, t = next(loader)
    all_io += time.time() - io
    
    if not args.no_compute:
        inp = b.float().cuda()
        target = t.long().cuda()

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
print('      IO Wait: {}'.format(all_io / args.count))

if all_compute > 0:
    print('      Compute: {}'.format(all_compute / args.count))
    print('Compute Speed: {}'.format(args.batch_size * args.count / all_compute))

print('done')

#sys.exit()
print('sys exit')
os._exit(0)
print('os exit')


