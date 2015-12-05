#include <iostream>
#include <vector>
#include <string>

#include "find_files.hpp"
#include "opencv2/core/core.hpp"
#include "pascal_processing.hpp"


void generateImageNames(const std::vector<std::string>& list, const std::string& stripPattern, std::vector<std::string>& out) {
    for (auto it = list.cbegin(), iend = list.cend(); it != iend; ++it) {
        size_t pos = (*it).find(stripPattern);
        if (pos != std::string::npos) {
            out.emplace_back(std::move( (*it).substr(0, pos) ));
        }
    }
}

static const char commandLineDescription[] = 
    "{ d |       | Dataset .jpg images directory }"
    "{ h | false | Show help                     }"
    "{ i |       | Input .mat files directory    }"
    "{ o |       | Output .png directory         }"

    "{   | rfs   | 513 | Net receptive field size, defaults to 513 }";

static void printHelp(std::ostream& os) {
    os << "Usage:" << std::endl
       << "\tmat_to_png -d path/to/dataset/images " 
       << "-i path/to/mat/results -o path/to/output/dir [-h --rfs]" 
       << std::endl;
}

int main(int argc, char** argv) {
    auto parser = cv::CommandLineParser(argc, argv, commandLineDescription);
    const auto inputDir = parser.get<cv::String>("i");
    const auto datasetDir = parser.get<cv::String>("d");
    const auto outputDir = parser.get<cv::String>("o");
    auto receptiveFieldSize = parser.get<int>("rfs");
    if (receptiveFieldSize <= 0) {
        receptiveFieldSize = 513; // magic
    }
    if (parser.get<bool>("h") == true) {
        printHelp(std::cout);
        return 0;
    }
    if ((inputDir.empty() == true) || (datasetDir.empty() == true) || 
        (outputDir.empty() == true)) 
    {
        printHelp(std::cout);
        return 0;   
    }

    std::vector<std::string> matFilesList;
    listDirectory(inputDir, "*.mat", true, matFilesList);
    
    std::vector<std::string> imagesList;
    generateImageNames(matFilesList, "_blob_0", imagesList);

    try {
        PascalProcessor pascalProcessor(21, receptiveFieldSize);
        pascalProcessor.convertMatToPng(matFilesList, imagesList, inputDir, datasetDir, outputDir);
        std::cout << "Done." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}