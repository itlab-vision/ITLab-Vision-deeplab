#include <iostream>
#include <fstream>

#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\video\video.hpp"

#include "movie_maker.hpp"

static const char argsDefs[] = 
    "{ | list             |     | Path to target images list file }"
    "{ | fps              | 30  |                                 }"
    "{ | fpi              | 1   | Frames per image                }"
    "{ | video            |     | Resulting video name            }"
    "{ | width            | 400 | Output video frame width        }"
    "{ | height           | 300 | Output video frame height       }";

void printHelp(std::ostream& os)
{
    os << "\tUsage: --list=path/to/list.txt --video=output/filename" << std::endl
       << "\t[--fpi=1, --width=400, --height=300 --fps=30]" << std::endl;
}

namespace ReturnCode
{
    enum
    {
        Success = 0,
        InputFileNotFound = 1,
        OutputFileNotSpecified = 2,
        VideoNotCreated = 3
    };
}; //namespace ReturnCode


int main(int argc, char* argv[])
{
    const int defaultFPI = 1, defaultFPS = 30,
              defaultWidth = 400, defaultHeight = 300;
    // read command line arguments
    cv::CommandLineParser parser(argc, argv, argsDefs);
    auto listFileName = parser.get<std::string>("list");
    auto videoFileName = parser.get<std::string>("video");

    // Check command line args
    if (listFileName.empty() == true)
    {
        std::cerr << "Input file is not specified." << std::endl;
        printHelp(std::cerr);
        return ReturnCode::InputFileNotFound;
    }
    if (videoFileName.empty() == true)
    {
        std::cerr << "Output file is not specified." << std::endl;
        printHelp(std::cerr);
        return ReturnCode::OutputFileNotSpecified;
    }
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

    std::ifstream list(listFileName);
    if (list.is_open() == false)
    {
        std::cerr << "File '" << listFileName
                  << "' not found. Exiting." << std::endl;
        return ReturnCode::InputFileNotFound;
    }
    
    MovieMaker maker(frameWidth, frameHeight, fpi, fps);
    try
    {
        maker.createVideo(list, videoFileName);
        std::cout << "Video successfully created at: '"
                  << videoFileName << "'." << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return ReturnCode::VideoNotCreated;
    }

    return ReturnCode::Success;
}
