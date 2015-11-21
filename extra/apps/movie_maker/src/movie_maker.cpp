#include "movie_maker.hpp"

void MovieMaker::createVideo(std::istream& list,
    const std::string& outputFileName)
{
	cv::VideoWriter videoWriter;

	std::string tempFile = cv::tempfile(
        outputFileName.substr(outputFileName.rfind('.')).c_str());
	std::cout << "Opening file '" << tempFile << "' for writing." << std::endl;
	if (videoWriter.open(outputFileName, CV_FOURCC('D', 'I', 'V', 'X'), fps,
        cv::Size(frameWidth, frameHeight)) == false)
    {
		throw std::exception("Failed to open file for video writing.");
	}

	size_t count = 0;
	while (list.good() == true)
    {
		std::string fileName;
		std::getline(list, fileName);

		cv::Mat image = cv::imread(fileName);
		preprocessImage(image);

		for(int i = 0; i < frameRepeat; ++i)
        {
			videoWriter.write(image);
		}
		count += frameRepeat;
	}
	videoWriter.release();

	std::cout << "Total frames written: " << count << std::endl;
}

void MovieMaker::createVideo(std::vector<std::vector<std::string> > &imagesSet,
            const std::string& outputFileName)
{
    if (imagesSet.size() == 0)
    {
        throw std::exception("List of image sets is empty.");
    }

    cv::VideoWriter videoWriter;

	std::string tempFile = cv::tempfile(
        outputFileName.substr(outputFileName.rfind('.')).c_str());
	std::cout << "Opening file '" << tempFile << "' for writing." << std::endl;
	if (videoWriter.open(outputFileName, CV_FOURCC('D', 'I', 'V', 'X'), fps,
            cv::Size(frameWidth, frameHeight)) == false)
    {
		throw std::exception("Failed to open file for video writing.");
	}

	size_t count = 0;
    int kSets = imagesSet.size();
    int kImages = imagesSet[0].size();
    int idx = 0;
	while (idx < kImages)
    {
		std::vector<cv::Mat> images;
        for (int i = 0; i < kSets; i++)
        {
            std::string fileName = imagesSet[i][idx];
            cv::Mat image = cv::imread(fileName);
            images.push_back(image);
        }
		
        cv::Mat frame;
        mergeImages(images, frame);
		preprocessImage(frame);
        
		for(int i = 0; i < frameRepeat; ++i)
        {
			videoWriter << frame;
		}
		count += frameRepeat;
        idx++;
	}
    videoWriter.release();

	std::cout << "Total frames written: " << count << std::endl;
}

void MovieMaker::mergeImages(const std::vector<cv::Mat> &images,
        cv::Mat &frame)
{
    if (images.size() == 0)
    {
        throw std::exception("List of image sets is empty.");
    }
    int frameWidth = 0, frameHeight = 0, kImages = images.size(),
        frameType = images[0].type();
    for (int i = 0; i < kImages; i++)
    {
        frameWidth += images[i].cols;
        if (images[i].rows > frameHeight)
        {
            frameHeight = images[i].rows;
        }
    }
    frame.create(frameHeight, frameWidth, frameType);
    cv::Rect roi;
    roi.x = 0;
    roi.y = 0;
    for (int i = 0; i < kImages; i++)
    {
        roi.width = images[i].cols;
        roi.height = images[i].rows;
        cv::Mat subFrame = frame(roi);
        images[i].copyTo(subFrame);
        roi.x += images[i].cols;        
    }   
}

void MovieMaker::preprocessImage(cv::Mat& image)
{
    if (image.rows < frameHeight && image.cols < frameWidth)
    {
        return;
    }

    float scale, scaleOx = 1.0f, scaleOy = 1.0f;
    scaleOx = (image.cols > frameWidth) ? 
              frameWidth / ((float)image.cols) : scaleOx;
    scaleOy = (image.rows > frameHeight) ? 
              frameHeight / ((float)image.rows) : scaleOy;
    scale = (scaleOx > scaleOy) ? scaleOy : scaleOx;

	cv::Mat resized;
    cv::resize(image, resized, cv::Size(), scale, scale);
	image = resized;
}
