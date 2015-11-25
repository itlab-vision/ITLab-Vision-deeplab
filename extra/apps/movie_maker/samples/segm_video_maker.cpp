#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\video\video.hpp"

#include "movie_maker.hpp"
#include "utilities.hpp"
#include "exception.hpp"

static const char argsDefs[] = 
    "{ i | imgs             |      | Path to dataset images                  }"
    "{ s | segmimgs         |      | Path to segmented images                }"
    "{ d | dcrfsegmimgs     |      | Path to segmented images after denseCRF }"
    "{   | fps              | 30   | Frames per second                       }"
    "{   | fpi              | 1    | Frames per image                        }"
    "{ o | video            |      | Resulting video name                    }"
    "{   | width            | 1200 | Output video frame width                }"
    "{   | height           | 600  | Output video frame height               }";

void printHelp(std::ostream& os)
{
    os << "Usage: -i path/to/jpeg -s results/net -d results/crf " 
       << "-o filename.avi" << std::endl 
       << "\t[--fpi=1, --width=1200, --height=600 --fps=30]"
       << std::endl;
}

namespace ReturnCode
{
    enum
    {
        Success = 0,
        InputFileNotFound = 1,
        OutputFileNotSpecified = 2
    };
};

int main(int argc, char* argv[])
{
    const int defaultFPI = 1, defaultFPS = 30,
              defaultWidth = 1200, defaultHeight = 600;
    const std::string imgsMask = "*.jpg", segmImgsMask = "*.png",
                      dcrfsSegmImgsMask = "*.png";
    // read command line arguments
    cv::CommandLineParser parser(argc, argv, argsDefs);
    auto imgsDir = parser.get<std::string>("imgs");
    auto segmImgsDir = parser.get<std::string>("segmimgs");
    auto dcrfSegmImgsDir = parser.get<std::string>("dcrfsegmimgs");
    auto videoFileName = parser.get<std::string>("video");
    auto fpi = 
        (parser.get<int>("fpi", false) < 1) ?
        defaultFPI : parser.get<int>("fpi");
    auto fps = 
        (parser.get<int>("fps", false) <= 0) ? 
        defaultFPS : parser.get<int>("fps");    
    auto frameWidth = 
        (parser.get<int>("width", false) <= 0) ? 
        defaultWidth : parser.get<int>("width");
    auto frameHeight = 
        (parser.get<int>("height", false) <= 0) ? 
        defaultHeight : parser.get<int>("height");
    
    // read initial and segmented images
    std::vector<std::string> images, segmImages, dcrfSegmImages;
    try
    {
        Utilities::GetFilesInFolder(imgsDir, imgsMask, images);
        Utilities::GetFilesInFolder(segmImgsDir, segmImgsMask, segmImages);
        Utilities::GetFilesInFolder(dcrfSegmImgsDir, dcrfsSegmImgsMask, 
            dcrfSegmImages);
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
        printHelp(std::cout);
        return 0;
    }

    // create vector of corresponding images
    std::vector<std::vector<std::string> > imgsSet;
    imgsSet.push_back(images);
    imgsSet.push_back(segmImages);
    imgsSet.push_back(dcrfSegmImages);
    // create vector of titles
    std::vector<std::string> setNames;
    setNames.push_back("Initial image");
    setNames.push_back("Segmented image (CNN)");
    setNames.push_back("Segmented image (CNN + denseCRF)");

    // create movie
    MovieMaker maker(frameWidth, frameHeight, fpi, fps);
    try
    {
        maker.createVideo(setNames, imgsSet, videoFileName);
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
        return 0;
    }
    return 1;
}
