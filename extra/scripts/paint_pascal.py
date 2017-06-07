"""
Usage: paint_pascal.py input/dir output/dir
Converts grayscale inference to palette-based with conformance:
0 -> (0, 0, 0)
1 -> (128, 0, 0)
2 -> (0, 128, 0)
3 -> (128, 128, 0)
4 -> (0, 0, 128)
5 -> (128, 0, 128)
6 -> (0, 128, 128)
7 -> (128, 128, 128)
8 -> (64, 0, 0)
9 -> (191, 0, 0)
10 -> (64, 128, 0)
11 -> (191, 128, 0)
12 -> (64, 0, 128)
13 -> (191, 0, 128)
14 -> (64, 127, 128)
15 -> (191, 128, 128)
16 -> (0, 64, 0)
17 -> (128, 64, 0)
18 -> (0, 191, 0)
19 -> (128, 191, 0)
20 -> (0, 64, 128)
"""


from PIL import Image
import sys
import os


def show_help():
    print(__doc__)


def class_conformance_r(c):
    if (c == 0):
        return 0
    if (c == 1):
        return 128
    if (c == 2):
        return 0
    if (c == 3):
        return 128
    if (c == 4):
        return 0
    if (c == 5):
        return 128
    if (c == 6):
        return 0
    if (c == 7):
        return 128
    if (c == 8):
        return 64
    if (c == 9):
        return 191
    if (c == 10):
        return 64
    if (c == 11):
        return 191
    if (c == 12):
        return 64
    if (c == 13):
        return 191
    if (c == 14):
        return 64
    if (c == 15):
        return 191
    if (c == 16):
        return 0
    if (c == 17):
        return 128
    if (c == 18):
        return 0
    if (c == 19):
        return 128
    if (c == 20):
        return 0
    return 0


def class_conformance_g(c):
    if (c == 0):
        return 0
    if (c == 1):
        return 0
    if (c == 2):
        return 128
    if (c == 3):
        return 128
    if (c == 4):
        return 0
    if (c == 5):
        return 0
    if (c == 6):
        return 128
    if (c == 7):
        return 128
    if (c == 8):
        return 0
    if (c == 9):
        return 0
    if (c == 10):
        return 128
    if (c == 11):
        return 128
    if (c == 12):
        return 0
    if (c == 13):
        return 0
    if (c == 14):
        return 127
    if (c == 15):
        return 128
    if (c == 16):
        return 64
    if (c == 17):
        return 64
    if (c == 18):
        return 191
    if (c == 19):
        return 191
    if (c == 20):
        return 64
    return 0


def class_conformance_b(c):
    if (c == 0):
        return 0
    if (c == 1):
        return 0
    if (c == 2):
        return 0
    if (c == 3):
        return 0
    if (c == 4):
        return 128
    if (c == 5):
        return 128
    if (c == 6):
        return 128
    if (c == 7):
        return 128
    if (c == 8):
        return 0
    if (c == 9):
        return 0
    if (c == 10):
        return 0
    if (c == 11):
        return 0
    if (c == 12):
        return 128
    if (c == 13):
        return 128
    if (c == 14):
        return 128
    if (c == 15):
        return 128
    if (c == 16):
        return 0
    if (c == 17):
        return 0
    if (c == 18):
        return 0
    if (c == 19):
        return 0
    if (c == 20):
        return 128
    return 0


if (__name__ == "__main__"):
    if (len(sys.argv) != 3):
        show_help()
        exit()

    input_directory = os.path.abspath(sys.argv[1])
    output_directory = os.path.abspath(sys.argv[2])

    for entry in os.listdir(input_directory):
        if (entry[-4:] != ".png"):
            continue

        print('Processing entry \'%s\'' % (entry))

        inference_filepath = os.path.join(input_directory, entry[:-4] + os.extsep + 'png')
        painted_inference_filepath = os.path.join(output_directory, entry[:-4] + os.extsep + 'png')

        image = Image.open(inference_filepath).convert("RGB").split()
        painted_channels = [None] * 3
        painted_channels[0] = image[0].point(class_conformance_r)
        painted_channels[1] = image[1].point(class_conformance_g)
        painted_channels[2] = image[2].point(class_conformance_b)
        Image.merge("RGB", painted_channels).save(painted_inference_filepath)