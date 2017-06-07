"""
Usage: paint_camvid.py input/dir output/dir
Converts grayscale inference to palette-based with conformance:
00. 0 0 0 Void
01. 192 0 128 Archway
02. 0 128 192 Bicyclist
03. 0 128 64 Bridge
04. 128 0 0  Building
05. 64 0 128 Car
06. 64 0 192 CartLuggagePram
07. 192 128 64 Child
08. 192 192 128 Column_Pole
09. 64 64 128 Fence
10. 128 0 192 LaneMkgsDriv
11. 192 0 64 LaneMkgsNonDriv
12. 128 128 64 Misc_Text
13. 192 0 192 MotorcycleScooter
14. 128 64 64 OtherMoving
15. 64 192 128 ParkingBlock
16. 64 64 0 Pedestrian
17. 128 64 128 Road
18. 128 128 192 RoadShoulder
19. 0 0 192 Sidewalk
20. 192 128 128 SignSymbol
21. 128 128 128 Sky
22. 64 128 192 SUVPickupTruck
23. 0 0 64 TrafficCone
24. 0 64 64 TrafficLight
25. 192 64 128 Train
26. 128 128 0 Tree
27. 192 128 192 Truck_Bus
28. 64 0 64 Tunnel
29. 192 192 0 VegetationMisc
30. 64 128 64 Animal
31. 64 192 0 Wall
"""


from PIL import Image
import sys
import os


def show_help():
    print(__doc__)


def class_conformance_r(p):
    if p == 0:
        return 0
    if p == 1:
        return 192
    if p == 2:
        return 0
    if p == 3:
        return 0
    if p == 4:
        return 128
    if p == 5:
        return 64
    if p == 6:
        return 64
    if p == 7:
        return 192
    if p == 8:
        return 192
    if p == 9:
        return 64
    if p == 10:
        return 128
    if p == 11:
        return 192
    if p == 12:
        return 128
    if p == 13:
        return 192
    if p == 14:
        return 128
    if p == 15:
        return 64
    if p == 16:
        return 64
    if p == 17:
        return 128
    if p == 18:
        return 128
    if p == 19:
        return 0
    if p == 20:
        return 192
    if p == 21:
        return 128
    if p == 22:
        return 64
    if p == 23:
        return 0
    if p == 24:
        return 0
    if p == 25:
        return 192
    if p == 26:
        return 128
    if p == 27:
        return 192
    if p == 28:
        return 64
    if p == 29:
        return 192
    if p == 30:
        return 64
    if p == 31:
        return 64
    return 0


def class_conformance_g(p):
    if p == 0:
        return 0
    if p == 1:
        return 0
    if p == 2:
        return 128
    if p == 3:
        return 128
    if p == 4:
        return 0
    if p == 5:
        return 0
    if p == 6:
        return 0
    if p == 7:
        return 128
    if p == 8:
        return 192
    if p == 9:
        return 64
    if p == 10:
        return 0
    if p == 11:
        return 0
    if p == 12:
        return 128
    if p == 13:
        return 0
    if p == 14:
        return 64
    if p == 15:
        return 192
    if p == 16:
        return 64
    if p == 17:
        return 64
    if p == 18:
        return 128
    if p == 19:
        return 0
    if p == 20:
        return 128
    if p == 21:
        return 128
    if p == 22:
        return 128
    if p == 23:
        return 0
    if p == 24:
        return 64
    if p == 25:
        return 64
    if p == 26:
        return 128
    if p == 27:
        return 128
    if p == 28:
        return 0
    if p == 29:
        return 192
    if p == 30:
        return 128
    if p == 31:
        return 192
    return 0


def class_conformance_b(p):
    if p == 0:
        return 0
    if p == 1:
        return 128
    if p == 2:
        return 192
    if p == 3:
        return 64
    if p == 4:
        return 0
    if p == 5:
        return 128
    if p == 6:
        return 192
    if p == 7:
        return 64
    if p == 8:
        return 128
    if p == 9:
        return 128
    if p == 10:
        return 192
    if p == 11:
        return 64
    if p == 12:
        return 64
    if p == 13:
        return 192
    if p == 14:
        return 64
    if p == 15:
        return 128
    if p == 16:
        return 0
    if p == 17:
        return 128
    if p == 18:
        return 192
    if p == 19:
        return 192
    if p == 20:
        return 128
    if p == 21:
        return 128
    if p == 22:
        return 192
    if p == 23:
        return 64
    if p == 24:
        return 64
    if p == 25:
        return 128
    if p == 26:
        return 0
    if p == 27:
        return 192
    if p == 28:
        return 64
    if p == 29:
        return 0
    if p == 30:
        return 64
    if p == 31:
        return 0
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