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
       << "\t[--fpi=<number>, --width=400, --height=300 --fps=30]" << std::endl;
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
    int framesPerImage = parser.get<int>("fpi");
    if (framesPerImage < 1)
    {
        framesPerImage = 1;
    }
    int fps = parser.get<int>("fps");
    if (fps <= 0)
    {
        fps = 30; //magic
    }
    int frameWidth = parser.get<int>("width");
    int frameHeight = parser.get<int>("height");
    if (frameWidth <= 0)
    {
        frameWidth = 400; //magic
    }
    if (frameHeight <= 0)
    {
        frameHeight = 300; //magic
    }

    std::ifstream list(listFileName);
    if (list.is_open() == false)
    {
        std::cerr << "File '" << listFileName
                  << "' not found. Exiting." << std::endl;
        return ReturnCode::InputFileNotFound;
    }
    
    MovieMaker maker(frameWidth, frameHeight, framesPerImage, fps);
    try
    {
        maker.createVideo(list, videoFileName);
        std::cout << "Video successfully created at: '"
                  << videoFileName << "'." << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
