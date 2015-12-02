#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>

#include <dirent.h>
#include <fnmatch.h>

#include "exception.hpp"
#include "mat_processing.hpp"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


void listDirectory(const std::string& path, const std::string& pattern, bool stripExtension, std::vector<std::string>& fileNames) {
    DIR* dir = opendir(path.c_str());
    if (dir == NULL) {
        throw exception("Error opening dir: '" + path + "'.");
    }

    dirent* entry = readdir(dir);
    if (entry == NULL) {
        throw exception("Directory : '" + path + "' is empty!");
    }

    while (entry != NULL) {
        DIR* testEntry = opendir(entry->d_name);
        
        if (testEntry == NULL) { // if not a directory
            if (fnmatch(pattern.c_str(), entry->d_name, 0) == 0) {
                if (stripExtension == true) {
                    std::string tmp(entry->d_name);                    
                    fileNames.emplace_back(std::move( tmp.substr(0, tmp.rfind(".")) ));
                } else {
                    fileNames.emplace_back(entry->d_name);
                }
            }
        } else {
            closedir(testEntry);
        }
    
        entry = readdir(dir);
    }

    closedir(dir);
}

void generateImageNames(const std::vector<std::string>& list, const std::string& stripPattern, std::vector<std::string>& out) {
    for (auto it = list.cbegin(), iend = list.cend(); it != iend; ++it) {
        size_t pos = (*it).find(stripPattern);
        if (pos != std::string::npos) {
            out.emplace_back(std::move( (*it).substr(0, pos) ));
        }
    }
}

static const char commandLineDescription[] = 
    "{ d |       | Dataset .jpg images        }"
    "{ h | false | Show help                  }"
    "{ i |       | Input .mat files directory }"
    "{ o |       | Output .png directory      }";


static void printHelp(std::ostream& os) {
    os << "Usage:" << std::endl
       << "\tmat_to_png -d path/to/dataset/images -i path/to/mat/results -o path/to/output/dir [-h]";
}

static const cv::Scalar pascalColors[] = {
    cv::Scalar(0.0, 0.0, 0.0),
    cv::Scalar(0.501960784313726, 0.0, 0.0),
    cv::Scalar(0.0,    0.501960784313726, 0.0),
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

int main(int argc, char** argv) {
    auto parser = cv::CommandLineParser(argc, argv, commandLineDescription);
    const auto inputDir = parser.get<cv::String>("i");
    const auto datasetDir = parser.get<cv::String>("d");
    const auto outputDir = parser.get<cv::String>("o");
    if (parser.get<bool>("h") == true) {
        printHelp(std::cout);
        return 0;
    }
    if ((inputDir.empty() == true) || (datasetDir.empty() == true) || (outputDir.empty() == true)) {
        printHelp(std::cout);
        return 1;   
    }

    std::vector<std::string> matFilesList;
    listDirectory(inputDir, "*.mat", true, matFilesList);
    
    std::vector<std::string> imagesList;
    generateImageNames(matFilesList, "_blob_0", imagesList);

    cv::Mat inferenceData(513 * 21, 513, CV_32F);
    for (size_t i = 0; i < matFilesList.size(); ++i) {
        if (i % 100 == 0) {
            std::cout << "Processing " << i << " entry from " << matFilesList.size() << "..." << std::endl;
        }

        const auto fileName = datasetDir + "/" + imagesList[i] + ".jpg";
        const auto datasetImage = cv::imread(fileName);
        if (datasetImage.empty() == true) {
            std::cerr << "Failed to open image: '" << fileName << "'. Skipping." << std::endl;
            continue;
        }  

        cv::Mat outputImage(datasetImage.rows, datasetImage.cols, CV_8UC3);

        float* inference = reinterpret_cast<float*>(inferenceData.data);
        int channels = 0;
        LoadMatFile(inputDir + "/" + matFilesList[i] + ".mat", inference, outputImage.rows, outputImage.cols, channels, true);

        for (int y = 0; y < outputImage.rows; ++y) {
            for (int x = 0; x < outputImage.cols; ++x) {
                const auto inferenceBegin = inference + (y * outputImage.cols + x) * channels;
                const auto maxChannel = std::min_element(inferenceBegin, inferenceBegin + channels); 
                const size_t colorIndex = std::distance(inferenceBegin, maxChannel);
                outputImage(cv::Range(y, y + 1), cv::Range(x, x + 1)).
                    setTo(255.0 * pascalColors[colorIndex]);
            }
        }
        cv::cvtColor(outputImage, outputImage, CV_RGB2BGR);
        cv::imwrite(outputDir + "/" + imagesList[i] + ".png", outputImage);
    }
    return 0;
}