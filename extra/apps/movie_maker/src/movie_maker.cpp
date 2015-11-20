#include "movie_maker.hpp"

void MovieMaker::createVideo(std::istream& list,
    const std::string& outputFileName) {
	cv::VideoWriter videoWriter;

	std::string tempFile = cv::tempfile(
        outputFileName.substr(outputFileName.rfind('.')).c_str());
	std::cout << "Opening file '" << tempFile << "' for writing." << std::endl;
	if (videoWriter.open(outputFileName, CV_FOURCC('D', 'I', 'V', 'X'), fps,
        cv::Size(frameWidth, frameHeight)) == false) {
		throw std::exception("Failed to open file for video writing.");
	}

	size_t count = 0;
	while (list.good() == true) {
		std::string fileName;
		std::getline(list, fileName);

		cv::Mat image = cv::imread(fileName);
		preprocessImage(image);

		for(int i = 0; i < frameRepeat; ++i) {
			videoWriter.write(image);
		}
		count += frameRepeat;
	}
	videoWriter.release();

	std::cout << "Total frames written: " << count << std::endl;
}

void MovieMaker::preprocessImage(cv::Mat& image) {
	if ((image.rows != frameHeight) || (image.cols != frameWidth)) {
		const int bottom = std::max(0, frameHeight - image.rows);
		const int right = std::max(0, frameWidth - image.cols);
		if ((right != 0) || (bottom != 0)) {
			cv::copyMakeBorder(image, image, 0, bottom, 0, right,
                cv::BORDER_CONSTANT, 0);
		}

		cv::Mat resized = cv::Mat::zeros(frameHeight, frameWidth, image.type());
		image(cv::Range(0, std::min(image.rows, frameHeight)),
			    cv::Range(0, std::min(image.cols, frameWidth))
			 ).copyTo(resized);
		image = resized;
	}
}
