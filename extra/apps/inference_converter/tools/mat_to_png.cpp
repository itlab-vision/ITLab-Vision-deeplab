#include <iostream>
#include <string>
#include <vector>

#include "find_files.hpp"
#include "opencv2/core/core.hpp"
#include "pascal_processing.hpp"


void generateImageNames(const std::vector<std::string>& list,
    const std::string& stripPattern, std::vector<std::string>& out)
{
    for (auto it = list.cbegin(), iend = list.cend(); it != iend; ++it) {
        size_t pos = (*it).find(stripPattern);
        if (pos != std::string::npos) {
            out.emplace_back(std::move( (*it).substr(0, pos) ));
        }
    }
}

static const char commandLineDescription[] =
    "{ b | false | Convert to grayscale(GT) format      }"
    "{ d |       | Dataset '.jpg' images directory      }"
    "{ h | false | Show help and exit                   }"
    "{ i |       | Input '.mat' files directory         }"
    "{ o |       | Output '.png' directory              }"
    "{ c | 21    | Class count, defaults to 21          }"
    "{ s | 513   | Net output map size, defaults to 513 }";

static void printHelp(std::ostream& os) {
    os << "Usage:"
         << "\tmat_to_png -d=path/to/dataset/images "
         << "-i=path/to/mat/results -o=path/to/output/dir "
         << "[-h -b -c=21 -s=513]"
         << std::endl
       << "\t-b\tSpecify output image type - grayscale or RGB" << std::endl
       << "\t-c\tClass count, equals to .mat maps count" << std::endl
       << "\t-d\tDataset '.jpg' images directory" << std::endl
       << "\t-h\tPrint this help and exit" << std::endl
       << "\t-i\tInput '.mat' files directory" << std::endl
       << "\t-o\tOutput '.png' files directory" << std::endl
       << "\t-s\tNet output map size" << std::endl
       << std::endl
       << "Important note: if class count in your dataset is"
         << " exceeds 21 " << std::endl
       << "then only grayscale (-b) conversion mode is working." << std::endl
       << std::endl;
}

int main(int argc, char** argv) {
    auto parser = cv::CommandLineParser(argc, argv, commandLineDescription);
    if (parser.get<bool>("h") == true) {
        printHelp(std::cout);
        return 0;
    }

    const auto inputDir = parser.get<cv::String>("i");
    const auto datasetDir = parser.get<cv::String>("d");
    const auto outputDir = parser.get<cv::String>("o");
    const bool doGrayscaleFormat = parser.get<bool>("b");
    auto outputSize = parser.get<int>("s");
    if (outputSize <= 1) {
        outputSize = 513;
    }
    auto classCount = parser.get<int>("c");
    if (classCount <= 1) {
        classCount = 21;
    }
    if ((inputDir.empty() == true) ||
        (datasetDir.empty() == true) ||
        (outputDir.empty() == true))
    {
        printHelp(std::cout);
        return 0;
    }

    std::cout << "Parameters:\t" << std::endl
              << "Net output:\t" << inputDir << std::endl
              << "Dataset dir:\t" << datasetDir << std::endl
              << "Output dir:\t" << outputDir << std::endl
              << "Grayscale:\t" << doGrayscaleFormat << std::endl;

    std::vector<std::string> matFilesList;
    listDirectory(inputDir, "*.mat", true, matFilesList);

    std::vector<std::string> imagesList;
    generateImageNames(matFilesList, "_blob_0", imagesList);

    try {
        PascalProcessor pascalProcessor(classCount, outputSize);
        pascalProcessor.convertMatToPng(matFilesList, imagesList, inputDir,
            datasetDir, outputDir, doGrayscaleFormat);
        std::cout << "Done." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
