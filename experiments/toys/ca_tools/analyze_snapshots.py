#!/usr/bin/env python3
"""
analyze_snapshots.py
Lightweight snapshot analyzer: computes basic per-image features and writes features.csv
Dependencies: Pillow, numpy
Install: pip install pillow numpy

Usage: python3 analyze_snapshots.py <snapshots_dir> [--out features.csv]

Features computed (per PNG):
- filename
- width, height
- mean_R, mean_G, mean_B
- std_R, std_G, std_B
- entropy_gray
- percent_bright (grayscale > mean+std*0.5)
- edge_density (sobel approximate)

"""
import sys
import os
import argparse
from PIL import Image
import numpy as np
import csv


def shannon_entropy(arr):
    # arr: 0..255
    vals,counts = np.unique(arr, return_counts=True)
    probs = counts / counts.sum()
    probs = probs[probs>0]
    return -np.sum(probs*np.log2(probs))


def edge_density_gray(gray):
    # sobel kernels
    kx = np.array([[1,0,-1],[2,0,-2],[1,0,-1]], dtype=float)
    ky = kx.T
    gx = np.abs(convolve2d(gray, kx))
    gy = np.abs(convolve2d(gray, ky))
    grad = np.sqrt(gx*gx + gy*gy)
    return np.mean(grad) / 255.0


def convolve2d(img, kernel):
    # simple 2D convolution with zero padding
    h, w = img.shape
    kh, kw = kernel.shape
    pad_h = kh//2
    pad_w = kw//2
    padded = np.pad(img, ((pad_h,pad_h),(pad_w,pad_w)), mode='reflect')
    out = np.zeros_like(img, dtype=float)
    for i in range(h):
        for j in range(w):
            patch = padded[i:i+kh, j:j+kw]
            out[i,j] = np.sum(patch * kernel)
    return out


def analyze_image(path):
    img = Image.open(path).convert('RGB')
    arr = np.array(img)
    h,w,_ = arr.shape
    R = arr[:,:,0].astype(float)
    G = arr[:,:,1].astype(float)
    B = arr[:,:,2].astype(float)
    meanR, meanG, meanB = R.mean(), G.mean(), B.mean()
    stdR, stdG, stdB = R.std(), G.std(), B.std()
    gray = (0.299*R + 0.587*G + 0.114*B).astype(np.uint8)
    entropy = shannon_entropy(gray.flatten())
    thresh = gray.mean() + 0.5*gray.std()
    percent_bright = np.mean(gray > thresh)
    ed = edge_density_gray(gray.astype(float))
    return {
        'width': w,
        'height': h,
        'meanR': float(meanR), 'meanG': float(meanG), 'meanB': float(meanB),
        'stdR': float(stdR), 'stdG': float(stdG), 'stdB': float(stdB),
        'entropy_gray': float(entropy), 'percent_bright': float(percent_bright), 'edge_density': float(ed)
    }


def main():
    p = argparse.ArgumentParser()
    p.add_argument('snapdir', help='directory with PNG snapshots')
    p.add_argument('--out', default='features.csv')
    args = p.parse_args()
    files = sorted([f for f in os.listdir(args.snapdir) if f.lower().endswith('.png')])
    if not files:
        print('No PNG files found in', args.snapdir)
        sys.exit(1)
    outpath = args.out
    with open(outpath, 'w', newline='') as csvfile:
        writer = None
        for fn in files:
            path = os.path.join(args.snapdir, fn)
            feats = analyze_image(path)
            row = {'filename': fn}
            row.update(feats)
            if writer is None:
                fieldnames = ['filename'] + list(feats.keys())
                writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
                writer.writeheader()
            writer.writerow(row)
    print('Features written to', outpath)

if __name__ == '__main__':
    main()
