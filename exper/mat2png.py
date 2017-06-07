"""
Usage: python mat2png.py input/dir output/dir [gt/dir]
This tool enables to convert densecrf module '.mat' inference to '.png' images.
Output format is grayscale png image.
"""

import scipy.io
import numpy as np
import sys
import os
from PIL import Image


def load_mat(file_path, data_type):
    mat = scipy.io.loadmat(file_path)['data']
    result = np.array(mat, dtype=data_type)
    return result


if (__name__ == '__main__'):
    input_dir = os.path.abspath(sys.argv[1])
    output_dir = os.path.abspath(sys.argv[2])
    gt_dir = None
    if (len(sys.argv) == 4):
    	gt_dir = os.path.abspath(sys.argv[3])

    for entry in os.listdir(input_dir):
        if (entry[-4:] != '.mat'):
            continue

        print ('Processing file \'%s\'...' % (entry))

        shape = None
        if (gt_dir != None):
        	shape = Image.open(gt_dir + "/" + entry[:-4] + os.path.extsep + 'jpg').size

        data_type = np.dtype(np.float)
        data = load_mat(os.path.join(input_dir, entry), data_type)
        data = np.argmax(data, 2)
        data = np.swapaxes(data, 0, 1)
        data = np.array(data, dtype='u1', order='C', copy=False)
        image = Image.frombytes('P', (data.shape[1], data.shape[0]), data)
        if (shape != None):
        	image = image.crop((0, 0, shape[0], shape[1]))
        image.save(os.path.join(output_dir, entry[:-4] + os.path.extsep + 'png'))