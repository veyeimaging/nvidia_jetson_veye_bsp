import argparse
import numpy as np
import cv2

# Define command-line arguments
parser = argparse.ArgumentParser(description='Read and display raw image file')
parser.add_argument('filename', type=str, help='raw image filename')
parser.add_argument('width', type=int, help='image width')
parser.add_argument('height', type=int, help='image height')
parser.add_argument('bits', type=int, help='bits per pixel (8, 10, or 12)')

# Parse command-line arguments
args = parser.parse_args()

# Read raw image data from file
with open(args.filename, 'rb') as f:
    if args.bits == 8:
        data = np.fromfile(f, dtype=np.uint8)
    elif args.bits == 10:
        data = np.fromfile(f, dtype=np.uint16)
        data = ((data >> 2) & 0x3FF).astype(np.uint8)
    elif args.bits == 12:
        data = np.fromfile(f, dtype=np.uint16)
        data = ((data >> 4) & 0xFF).astype(np.uint8)

# reshape image data into a two-dimensional array:
image_data = data.reshape(args.height, args.width)

# Display image
cv2.imshow('Image', image_data)
cv2.waitKey(0)
cv2.destroyAllWindows()
