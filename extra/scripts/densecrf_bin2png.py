import io
import numpy as np
import sys
import os
from PIL import Image


def load_binary(file_path, data_type):
    result = None
    try:
        file = open(file_path, 'rb')

        row = int.from_bytes(file.read(4), byteorder = 'little', signed = True)
        col = int.from_bytes(file.read(4), byteorder = 'little', signed = True)
        channel = int.from_bytes(file.read(4), byteorder = 'little', signed = True)
        data_length = row * col * channel

        data = np.fromfile(file, count = data_length, dtype = data_type)
        result = np.reshape(data, [col, row, channel]);
    finally:
        file.close()
    return result


if (__name__ == '__main__'):
    input_dir = os.path.abspath(sys.argv[1])
    output_dir = os.path.abspath(sys.argv[2])

    for entry in os.listdir(input_dir):
        if (entry[-4:] != '.bin'):
            continue

        print ('Processing file \'%s\'...' % (entry))

        data_type = np.dtype('<i2')
        data = load_binary(os.path.join(input_dir, entry), data_type)
        data = np.swapaxes(data, 0, 1)
        data = np.array(data, dtype = 'u1', order = 'C', copy = False)
        image = Image.frombytes('P', (data.shape[1], data.shape[0]), data)
        image.save(os.path.join(output_dir, entry[:-4] + os.path.extsep + 'png'))