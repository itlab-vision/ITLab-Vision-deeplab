#include "pascal_processing.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "exception.hpp"
#include "mat_processing.hpp"


void convertSegmentationToPascal(const float* inference, size_t channels, cv::Mat& outputImage);
void convertSegmentationToPascal(const float* inference, size_t channels, cv::Mat& outputImage, const cv::Scalar* colormap);


const cv::Scalar PascalProcessor::pascalColors[] = {
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

PascalProcessor::PascalProcessor(size_t classCount, size_t receptiveFieldSize) :
    pascalClassCount(classCount), netReceptiveFieldSize(receptiveFieldSize)
{}

void PascalProcessor::convertMatToPng(
            const std::vector<std::string>& matFileNames, 
            const std::vector<std::string>& imageFileNames,
            const std::string& inferenceDir,
            const std::string& datasetDir,
            const std::string& outputDir,
            bool grayscale
            )
{
    //preallocate memory for images
    const size_t inferenceDataSize = netReceptiveFieldSize * netReceptiveFieldSize *
                                     pascalClassCount;
    float* inference = new float[inferenceDataSize];

    const size_t matBufferSize = netReceptiveFieldSize * netReceptiveFieldSize *
                                 pascalClassCount * sizeof(float);
    void* matBuffer = malloc(matBufferSize);

    cv::Mat outputImage(netReceptiveFieldSize, netReceptiveFieldSize, CV_8UC3);
    for (size_t i = 0; i < matFileNames.size(); ++i) {
        if (i % 100 == 0) {
            std::cout << "Processing " << i << " entry from "
                      << matFileNames.size() << "..." << std::endl;
        }

        const auto fileName = datasetDir + "/" + imageFileNames[i] + ".jpg";
        const auto datasetImage = cv::imread(fileName);
        if (datasetImage.empty() == true) {
            std::cerr << "Failed to open image: '" << fileName << "'." << std::endl;
            continue;
        }
        
        int channels = 0;
        LoadMatFile(inferenceDir + "/" + matFileNames[i] + ".mat", 
                    inference, inferenceDataSize, datasetImage.rows, datasetImage.cols,
                    channels, true, matBuffer, matBufferSize
                   );

        if (grayscale == true) {
            outputImage.create(datasetImage.rows, datasetImage.cols, CV_8UC1);
        } else {
            outputImage.create(datasetImage.rows, datasetImage.cols, CV_8UC3);
        }
        PascalProcessor::convertSegmentationToPascal(inference, channels, outputImage); 
        
        writeSegmentedImage(outputDir + "/" + imageFileNames[i] + ".png", outputImage);
    }

    free(matBuffer);
    delete[] inference;
}

void PascalProcessor::writeSegmentedImage(const std::string& fileName, const cv::Mat& data) {
    cv::imwrite(fileName, data);
}

void PascalProcessor::convertSegmentationToPascal(const float* inference, size_t channels, cv::Mat& outputImage) {
    if (outputImage.channels() == 1) {
        ::convertSegmentationToPascal(inference, channels, outputImage);
    } else if (outputImage.channels() == 3) {
        ::convertSegmentationToPascal(inference, channels, outputImage, pascalColors);
    } else {
        throw exception("Unexpected output image channel count. 1 or 3 is expected.");
    }
}

void convertSegmentationToPascal(const float* inference, size_t channels, cv::Mat& outputImage) { 
#pragma omp parallel for   
    for (int y = 0; y < outputImage.rows; ++y) {
#pragma omp parallel for
        for (int x = 0; x < outputImage.cols; ++x) {
            const auto inferenceBegin = inference + (y * outputImage.cols + x) * channels;      
            const auto maxChannel = std::max_element(inferenceBegin, inferenceBegin + channels);
            const size_t colorIndex = std::distance(inferenceBegin, maxChannel);
            auto point = outputImage.data + (y * outputImage.cols + x) * outputImage.channels();        
            *point = colorIndex;
        }
    }
}

void convertSegmentationToPascal(const float* inference, size_t channels, cv::Mat& outputImage, const cv::Scalar* pascalColors) { 
#pragma omp parallel for   
    for (int y = 0; y < outputImage.rows; ++y) {
#pragma omp parallel for
        for (int x = 0; x < outputImage.cols; ++x) {
            const auto inferenceBegin = inference + (y * outputImage.cols + x) * channels;		
            const auto maxChannel = std::max_element(inferenceBegin, inferenceBegin + channels);
            const size_t colorIndex = std::distance(inferenceBegin, maxChannel);
            auto point = outputImage.data + (y * outputImage.cols + x) * outputImage.channels();        
            point[0] = cv::saturate_cast<uchar>(255.0 * pascalColors[colorIndex](2));
            point[1] = cv::saturate_cast<uchar>(255.0 * pascalColors[colorIndex](1));
            point[2] = cv::saturate_cast<uchar>(255.0 * pascalColors[colorIndex](0));
        }
    }
}

