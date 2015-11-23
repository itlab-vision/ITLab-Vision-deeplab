#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\video\video.hpp"

#include "movie_maker.hpp"
#include "utilities.hpp"

static const char argsDefs[] = 
    "{ | imgs             |      | Path to target images                   }"
    "{ | segmimgs         |      | Path to segmented images                }"
    "{ | dcrfsegmimgs     |      | Path to segmented images after denseCRF }"
    "{ | fps              | 30   | Frames per second                       }"
    "{ | fpi              | 1    | Frames per image                        }"
    "{ | video            |      | Resulting video name                    }"
    "{ | width            | 1200 | Output video frame width                }"
    "{ | height           | 600  | Output video frame height               }";

void printHelp(std::ostream& os)
{
    os << "\tUsage: --list=path/to/list.txt --video=output/filename" << std::endl
       << "\t[--fpi=<number>, --width=1200, --height=600 --fps=30]" << std::endl;
}

namespace ReturnCode
{
    enum
    {
        Success = 0,
        InputFileNotFound = 1,
        OutputFileNotSpecified = 2
    };
}; //namespace ReturnCode

int main(int argc, char* argv[])
{
    // read commanl line arguments
    cv::CommandLineParser parser(argc, argv, argsDefs);
    auto imgsDir = parser.get<std::string>("imgs");
    auto segmImgsDir = parser.get<std::string>("segmimgs");
    auto dcrfSegmImgsDir = parser.get<std::string>("dcrfsegmimgs");
    auto videoFileName = parser.get<std::string>("video");
    auto fpi = (parser.get<int>("fpi") < 1) ? 1 : parser.get<int>("fpi");
    auto fps = (parser.get<int>("fps") <= 0) ? 30 : parser.get<int>("fps");    
    auto frameWidth = 
        (parser.get<int>("width") <= 0) ? 1200 : parser.get<int>("width");
    auto frameHeight = 
        (parser.get<int>("height") <= 0) ? 600 : parser.get<int>("height");
    
    // read initial and segmented images
    std::vector<std::string> images, segmImages, dcrfSegmImages;
    Utilities::GetFilesInFolder(imgsDir, "*.jpg", images);
    Utilities::GetFilesInFolder(segmImgsDir, "*.jpg", segmImages);
    Utilities::GetFilesInFolder(dcrfSegmImgsDir, "*.jpg", dcrfSegmImages);
    std::vector<std::vector<std::string> > imgsSet;
    imgsSet.push_back(images);
    imgsSet.push_back(segmImages);
    imgsSet.push_back(dcrfSegmImages);

    // create movie
    MovieMaker maker(frameWidth, frameHeight, fpi, fps);
    maker.createVideo(imgsSet, videoFileName);
    return 0;
}
