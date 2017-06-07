"""
Usage: python segmentation_accuracy.py segmentation/dir gt/dir images_list.txt

This tool calculates a IoU score for set of given images
using a formula equivalent to intersection / union (with pieces of dark magic).
The result is the same as in Pascal VOC 2012 Challenge scripts.
"""


from PIL import Image, ImageStat
import sys
import os
import numpy


def show_help():
    print(__doc__)


class SegmentationResultsProcessor:
    """
    For customizing a set of existing classes specify
    classes field.

    results directory - a full path to results folder
    gt directory - a full path to ground truth folder
    list path - a full path to images list (without extensions)
    """

    classes = []
    results_directory = None
    gt_directory = None
    list_path = None

    def check_segmentation(self, image):
        """
        Returns an image intersection and union scores
        """

        assert(image.mode in ['1', 'L', 'P'])
        image_mask = image.point(
            lambda p: (len(self.classes) <= p) and (p < 255))

        stat = ImageStat.Stat(image_mask)
        if (0 < stat.sum[0]):
            raise Exception('Image has an unexpected class index')

    def process(self):
        """
        Processes the given set of images and returns an IoU score.
        Throws an exception on wrong input parameters
        """

        if (self.results_directory is None):
            raise Exception('Segmentation directory is not set')
        if (self.gt_directory is None):
            raise Exception('Ground Truth directory is not set')
        if (self.list_path is None):
            raise Exception('Images list path is not set')

        images_list = open(self.list_path, 'r')
        confcounts = numpy.zeros((len(self.classes), len(self.classes)))

        for entry in images_list:
            entry = entry.strip()
            print('Processing entry \'%s\'' % (entry))

            result_filepath = os.path.join(
                self.results_directory, entry + os.extsep + 'png')
            gt_filepath = os.path.join(
                self.gt_directory, entry + os.extsep + 'png')

            # load groudtruth image
            gtim = numpy.asarray(
                Image.open(gt_filepath).convert('P')).astype(float)
            # load segmentation image
            resim = numpy.asarray(
                Image.open(result_filepath).convert('P')).astype(float)

            if ((resim.shape[0] != gtim.shape[0]) or
                    (resim.shape[1] != gtim.shape[1])):
                raise Exception("Image sizes are different")

            sumim = 1 + gtim + resim * len(self.classes)
            locs = gtim < 255
            maskedsumim = numpy.extract(locs, sumim)
            [hs, bin_edges] = numpy.histogram(
                maskedsumim,
                bins=range(1, len(self.classes) * len(self.classes) + 2))
            confcounts += numpy.reshape(hs,
                                        (len(self.classes), len(self.classes)))

        self.overall_accuracy = 100 * \
            numpy.sum(numpy.diagonal(confcounts)) / numpy.sum(confcounts)

        # class accuracy
        self.class_acc = numpy.zeros(len(self.classes))
        denoms = numpy.sum(confcounts, axis=0)
        for i in range(0, len(self.classes)):
            if (denoms[i] == 0):
                denoms[i] = 1
            self.class_acc[i] = 100 * confcounts[i][i] / denoms[i]
        self.aver_class_acc = numpy.sum(self.class_acc) / len(self.classes)

        # pixel IoU
        self.accuracies = numpy.zeros(len(self.classes))
        gtj = numpy.sum(confcounts, axis=1)
        resj = numpy.sum(confcounts, axis=0)
        for j in range(0, len(self.classes)):
            gtjresj = confcounts[j][j]
            denom = gtj[j] + resj[j] - gtjresj
            if (denom == 0):
                denom = 1
            self.accuracies[j] = 100 * gtjresj / denom
        self.aver_accuracy = numpy.sum(self.accuracies) / len(self.classes)

    def show_results(self):
        print('----------------------------------')
        print('Percentage of pixels correctly labelled overall: %6.3f%%' %
              self.overall_accuracy)
        print('----------------------------------')
        print('Mean Class Accuracy:: %6.3f%%' % self.aver_class_acc)
        print('Per class accuracy:')
        for i in range(len(self.classes)):
            print('  %14s: %6.3f%%' %
                  (self.classes[i], self.class_acc[i]))
        print('----------------------------------')
        print('Average accuracy: %6.3f%%' % self.aver_accuracy)
        print('Accuracy for each class (intersection/union measure)')
        for i in range(len(self.classes)):
            print('  %14s: %6.3f%%' %
                  (self.classes[i], self.accuracies[i]))
        print('----------------------------------')


if (__name__ == '__main__'):
    if (len(sys.argv) != 4):
        show_help()
        exit()

    processor = SegmentationResultsProcessor()
    processor.results_directory = os.path.abspath(sys.argv[1])
    processor.gt_directory = os.path.abspath(sys.argv[2])
    processor.list_path = os.path.abspath(sys.argv[3])
    processor.classes = [
        'Void',
        'Archway',
        'Bicyclist',
        'Bridge',
        'Building',
        'Car',
        'CartLuggagePram',
        'Child',
        'Column_Pole',
        'Fence',
        'LaneMkgsDriv',
        'LaneMkgsNonDriv',
        'Misc_Text',
        'MotorcycleScooter',
        'OtherMoving',
        'ParkingBlock',
        'Pedestrian',
        'Road',
        'RoadShoulder',
        'Sidewalk',
        'SignSymbol',
        'Sky',
        'SUVPickupTruck',
        'TrafficCone',
        'TrafficLight',
        'Train',
        'Tree',
        'Truck_Bus',
        'Tunnel',
        'VegetationMisc',
        'Animal',
        'Wall'
    ]
    processor.process()
    processor.show_results()
