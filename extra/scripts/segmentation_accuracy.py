'''
Usage: python segmentation_accuracy.py segmentation/dir gt/dir images_list.txt

'''


from PIL import Image, ImageStat
import sys
import os



def show_help():
	print(__doc__)


class SegmentationResultsProcessor:
	Classes = [
		'background',
		'aeroplane',
		'bicycle',
		'bird',
		'boat',
		'bottle',
		'bus',
		'car',
		'cat',
		'chair',
		'cow',
		'diningtable',
		'dog',
		'horse',
		'motorbike',
		'person',
		'pottedplant',
		'sheep',
		'sofa',
		'train',
		'tvmonitor'
	]

	results_directory = None
	gt_directory = None
	list_path = None


	def calculate_image_IoU(self, image, gt, class_index):
		assert(image.mode in ['1', 'L', 'P'])
		assert(gt.mode in ['1', 'L', 'P'])
		
		image_mask = image.point(lambda p: (p == class_index) * 255)
		gt_mask = gt.point(lambda p: (p == class_index) * 255)
		gt_mask = gt_mask.convert('1')

		intersection = Image.new('1', image.size, 0)
		intersection.paste(image_mask, gt_mask)

		stat = ImageStat.Stat(intersection)
		intersection_count = stat.sum[0] / 255

		stat = ImageStat.Stat(image_mask)
		image_count = stat.sum[0] / 255

		stat = ImageStat.Stat(gt_mask)
		gt_count = stat.sum[0] / 255

		union_count = image_count + gt_count - intersection_count
		IoU = 0
		if (union_count != 0):
			IoU = intersection_count / union_count

		presence = (gt_count != 0)

		return (IoU, presence)


	def check_segmentation(self, image):
		assert(image.mode in ['1', 'L', 'P'])
		image_mask = image.point(lambda p: (len(self.Classes) <= p) and (p < 255))
		stat = ImageStat.Stat(image_mask)

		if (0 < stat.sum[0]):
			raise Exception('Image has an unexpected class index')


	def process_image(self, image_path, gt_path):
		IoUs = [(0.0, 0)] * len(self.Classes) # (IoU, class presence)
		
		image = Image.open(image_path)
		image = image.convert('L') # grayscale
		self.check_segmentation(image)

		gt = Image.open(gt_path)
		gt = gt.convert('P') # palette-based
		gt = gt.point(lambda p: (p < 255) * p)
		self.check_segmentation(gt)

		if (image.width != gt.width or image.height != gt.height):
			raise Exception("Image sizes are different")

		for class_index in range(len(self.Classes)):
			IoUs[class_index] = self.calculate_image_IoU(image, gt, class_index)

		return IoUs


	def process(self):
		if (self.results_directory == None):
			raise Exception('Segmentation directory is not set')
		if (self.gt_directory == None):
			raise Exception('Ground Truth directory is not set')
		if (self.list_path == None):
			raise Exception('Images list path is not set')

		files_count = 0
		classes_metrics = [0.0] * len(self.Classes)
		classes_presented = [0] * len(self.Classes)
		
		images_list = open(self.list_path, 'r')

		for entry in images_list:
			entry = entry.strip()
			print('Processing entry \'%s\'' % (entry))

			result_filepath = os.path.join(self.results_directory, entry + os.extsep + 'png')
			gt_filepath = os.path.join(self.gt_directory, entry + os.extsep + 'png')

			current_metrics = self.process_image(result_filepath, gt_filepath)
			for i in range(len(self.Classes)):
				classes_metrics[i] += current_metrics[i][0]
				classes_presented[i] += current_metrics[i][1]

			files_count += 1


		overall_accuracy = 0.0
		overall_classes_presented = 0
		for i in range(len(self.Classes)):
			if (0 < classes_presented[i]):
				classes_metrics[i] /= classes_presented[i]

		for i in range(1, len(self.Classes)):				
			overall_classes_presented += (classes_presented[i] != 0)
			overall_accuracy += classes_metrics[i]

		if (0 < overall_classes_presented):
			overall_accuracy /= overall_classes_presented

		self.classes_metrics = classes_metrics
		self.overall_accuracy = overall_accuracy


	def show_results(self):
		print('Segmentation accuracy (IoU metric)')
		print('Overall accuracy: %6.3f%%' % (100.0 * self.overall_accuracy))
		print('Per class accuracy:')
		for i in range(len(self.Classes)):
			print('  %14s: %6.3f%%' % \
				(self.Classes[i] , 100.0 * self.classes_metrics[i]) )


if (__name__ == '__main__'):
	if (len(sys.argv) <= 1):
		show_help()
		exit()

	processor = SegmentationResultsProcessor()
	processor.results_directory = os.path.abspath(sys.argv[1])
	processor.gt_directory = os.path.abspath(sys.argv[2])
	processor.list_path = os.path.abspath(sys.argv[3])
	processor.process()
	processor.show_results()