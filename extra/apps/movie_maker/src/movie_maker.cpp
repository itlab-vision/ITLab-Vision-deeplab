#include "movie_maker.hpp"
#include "exception.hpp"

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
        throw exception("Failed to open file for video writing.");
    }

    size_t count = 0;
    while (list.good() == true)
    {
        std::string fileName;
        std::getline(list, fileName);

        cv::Mat image = cv::imread(fileName);
        preprocessImage(image);

        for (int i = 0; i < frameRepeat; ++i)
        {
            videoWriter.write(image);
        }
        count += frameRepeat;
    }
    videoWriter.release();

    std::cout << "Total frames written: " << count << std::endl;
}

bool MovieMaker::checkEqualSize(
    const std::vector<std::vector<std::string> > &imagesSet)
{
    int kSets = imagesSet.size();
    for (int i = 0; i < kSets - 1; i++)
    {
        if (imagesSet[i].size() != imagesSet[i + 1].size())
        {
            return false;
        }
    }
    return true;
}

void MovieMaker::getCorrespondingImages(
    const std::vector<std::vector<std::string> > &imagesSet,
    const int idx, std::vector<cv::Mat> &images)
{
    int kSets = imagesSet.size();
    for (int i = 0; i < kSets; i++)
    {
        std::string fileName = imagesSet[i][idx];
        cv::Mat image = cv::imread(fileName);
        images.push_back(image);
    }
}

void MovieMaker::appendTitle(const std::vector<std::string> &setNames, 
            cv::Mat &imgsFrame)
{
    const int height = 40, width = imgsFrame.cols, 
              shiftOx = 5, shiftOy = 20,
              step = imgsFrame.cols / setNames.size();
    cv::Rect titleRoi(0, 0, width, height), 
             imgsRoi(0, height, imgsFrame.cols, imgsFrame.rows);
    cv::Mat frame = cv::Mat::zeros(imgsFrame.rows + height, imgsFrame.cols,
                                   imgsFrame.type());
    cv::Mat titleSubFrame = frame(titleRoi);
    imgsFrame.copyTo(frame(imgsRoi));
    for (int i = 0; i < setNames.size(); i++)
    {
        cv::Point cursor(i * step + shiftOx, shiftOy);
        cv::putText(titleSubFrame, setNames[i], cursor, 
                    cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8,
                    cv::Scalar(255, 255, 255));
    }
    imgsFrame = frame;
}

void MovieMaker::createVideo(const std::vector<std::string> &setNames,
        const std::vector<std::vector<std::string> > &imagesSet,
        const std::string &outputFileName)
{    
    if (imagesSet.size() == 0)
    {
        throw exception("List of image sets is empty.");
    }  
    if (!checkEqualSize(imagesSet))
    {
        throw exception("Sets of images should have the same size.");
    }
    
    // create legend
    cv::Mat legend;
    createVOCLegend(legend);
    
    // extract corresponding images    
    std::vector<cv::Mat> images;
    getCorrespondingImages(imagesSet, 0, images);
    
    // prepare first frame for writing
    cv::Mat imgsFrame;
    mergeImages(images, imgsFrame);
    preprocessImage(imgsFrame);
    appendTitle(setNames, imgsFrame);
    cv::Mat frame(imgsFrame.rows + legend.rows,
        (imgsFrame.cols > legend.cols) ? imgsFrame.cols : legend.cols,
        CV_8UC3);
    cv::Rect imgsRect(0, 0, frame.cols, imgsFrame.rows),
                legendRect(0, imgsFrame.rows, frame.cols, legend.rows);
    imgsFrame.copyTo(frame(imgsRect));
    legend.copyTo(frame(legendRect));
    
    //create video writer
    cv::VideoWriter videoWriter;
    videoWriter.open(outputFileName, CV_FOURCC('D', 'I', 'V', 'X'), fps,
        cv::Size(frame.cols, frame.rows));
    if (!videoWriter.isOpened())
    {
        throw exception("Failed to open file for video writing.");
    }

    // write first frame
    for(int i = 0; i < frameRepeat; ++i)
    {
        videoWriter << frame;
    }
    size_t count = frameRepeat;
    int idx = 1;

    // write other frames
    int kImages = imagesSet[0].size();
    int kSets = imagesSet.size();
    while (idx < kImages)
    {
        // extract corresponding images
        images.clear();
        getCorrespondingImages(imagesSet, idx, images);
        // prepare frame for writing
        cv::Mat imgsFrame;
        mergeImages(images, imgsFrame);
        preprocessImage(imgsFrame);
        appendTitle(setNames, imgsFrame);
        cv::Mat frame(imgsFrame.rows + legend.rows,
            (imgsFrame.cols > legend.cols) ? imgsFrame.cols : legend.cols,
            CV_8UC3);
        cv::Rect imgsRect(0, 0, frame.cols, imgsFrame.rows),
                 legendRect(0, imgsFrame.rows, frame.cols, legend.rows);
        imgsFrame.copyTo(frame(imgsRect));
        legend.copyTo(frame(legendRect));
        // write frame
        for (int i = 0; i < frameRepeat; ++i)
        {
            videoWriter << frame;
        }
        count += frameRepeat;
        idx++;
    }
    videoWriter.release();

    std::cout << "Total number of frames: " << count << std::endl;
}

void MovieMaker::mergeImages(const std::vector<cv::Mat> &images,
        cv::Mat &frame)
{
    if (images.size() == 0)
    {
        throw exception("List of image sets is empty.");
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
    scale   = (scaleOx > scaleOy) ? scaleOy : scaleOx;

    cv::Mat resized;
    cv::resize(image, resized, cv::Size(), scale, scale);
    image = resized;
}

void MovieMaker::createVOCLegend(cv::Mat &legend)
{
    const int kClasses = 21;
    const char *classes [] = {
        "bg", "aeroplane", "bicycle", "bird", "boat", "bottle", "bus",
        "car", "cat", "chair", "cow", "dining table", "dog", "horse",
        "motorbike", "person", "pottedplant", "sheep", "sofa",
        "train", "monitor" 
    };
    const cv::Scalar colors[] = {
        cv::Scalar(0.0, 0.0, 0.0),
        cv::Scalar(0.501960784313726, 0.0, 0.0),
        cv::Scalar(0.0,	0.501960784313726, 0.0),
        cv::Scalar(0.501960784313726, 0.501960784313726, 0.0), 
        cv::Scalar(0.0, 0.0, 0.501960784313726),
        cv::Scalar(0.501960784313726, 0.0, 0.501960784313726),
        cv::Scalar(0.0, 0.501960784313726, 0.501960784313726),
        cv::Scalar(0.501960784313726, 0.501960784313726, 0.501960784313726),
        cv::Scalar(0.250980392156863, 0.0, 0.0),
        cv::Scalar(0.752941176470588, 0.0, 0.0),
        cv::Scalar(0.250980392156863, 0.501960784313726, 0.0),
        cv::Scalar(0.752941176470588, 0.501960784313726, 0.0),
        cv::Scalar(0.250980392156863, 0.0, 0.501960784313726),
        cv::Scalar(0.752941176470588, 0.0, 0.501960784313726),
        cv::Scalar(0.250980392156863, 0.50196078431372, 0.501960784313726),
        cv::Scalar(0.752941176470588, 0.501960784313726, 0.501960784313726),
        cv::Scalar(0.0, 0.250980392156863, 0.0),
        cv::Scalar(0.501960784313726, 0.250980392156863, 0.0),
        cv::Scalar(0.0, 0.752941176470588, 0.0),
        cv::Scalar(0.501960784313726, 0.752941176470588, 0.0),
        cv::Scalar(0.0, 0.250980392156863, 0.501960784313726)
    };

    const int borderX = 5, borderY = 5;
    const int kGroups = 3, groupSize = kClasses / kGroups;    
    
    const int widthWithBorders = frameWidth,
              heightWithBorders = frameHeight / 2;
    const int width = widthWithBorders - 2 * kGroups * borderX, 
              height = heightWithBorders - 2 * kGroups * borderY;
    
    const int step = width / kGroups + 2 * borderX;
    const int stepHeight = height / groupSize, rectWidth = 30,
              shiftX = 5, shiftY = rectWidth / 2;

    legend = cv::Mat(heightWithBorders, widthWithBorders, CV_32FC3);
    for (int i = 0; i < kGroups; i++)
    {
        cv::Rect roi(i * step, 0, step, height + 2 * borderY);
        cv::Mat group = legend(roi);
        for (int j = 0; j < groupSize; j++)
        {
            int classIdx = i * groupSize + j;
            cv::rectangle(group, cv::Rect(borderX, borderY + j * stepHeight, 
                                          rectWidth, stepHeight), 
                          cv::Scalar(colors[classIdx][2], colors[classIdx][1],
                          colors[classIdx][0]), -1);
            cv::putText(group, classes[classIdx], 
                        cv::Point(borderX + rectWidth + shiftX, 
                                  borderY + j * stepHeight + 3 * shiftY / 2),
                        cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8,
                        cv::Scalar(1.0, 1.0, 1.0));
        }
    }
    double minVal, maxVal;
    cv::minMaxLoc(legend, &minVal, &maxVal);
    legend.convertTo(legend, CV_8UC3, 255);
}
