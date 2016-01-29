#include <iostream>

#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\video\video.hpp"

#include "utilities.h"

static const char argsDefs[] = 
    "{ v | video |     | Video file name. }"
    "{ d | dir   |     | Directory to save frames. }"
    "{ e | ext   | png | Images extension.}";

void printHelp(std::ostream& os)
{
    os << "Usage: -v path/to/video -d dir/to/imgs -e imgs/ext"
       << std::endl;
}

namespace ReturnCode
{
    enum
    {
        Success = 0,
        IncorrectInputParameters = 1,
        VideoFileNotOpened = 2
    };
};

int main(int argc, char **argv)
{
    cv::CommandLineParser parser(argc, argv, argsDefs);
    std::string videoFileName = parser.get<std::string>("video");
    std::string imgsDirName = parser.get<std::string>("dir");
    if (videoFileName.empty() || imgsDirName.empty())
    {
        std::cerr << "Incorrect input parameters." << std::endl;
        printHelp(std::cerr);
        return ReturnCode::IncorrectInputParameters;
    }
    std::string imgsExt = (parser.get<std::string>("ext").empty()) ? 
        "png" : parser.get<std::string>("ext");
    cv::VideoCapture capture(videoFileName);
    if (!capture.isOpened())
    {
        std::cerr << "Video file was not opened." << std::endl;
        printHelp(std::cerr);
        return ReturnCode::VideoFileNotOpened;
    }
    int frameIdx = 0;
    cv::Mat frame;
    capture >> frame;
    while (!frame.empty())
    {
        std::string fileName;
        getImageNameByIdx(frameIdx, imgsExt, fileName);
        std::ostringstream oss;
        oss << imgsDirName << "/" << fileName;
        cv::imwrite(oss.str(), frame);
        capture >> frame;
        frameIdx++;
    }
    capture.release();
    return ReturnCode::Success;
}
