#ifndef PASCAL_PROCESSING_HPP
#define PASCAL_PROCESSING_HPP

#include <string>
#include <vector>

#include "opencv2/core/core.hpp"


class PascalProcessor {
public:
	PascalProcessor(size_t classCount = 21, size_t mapSize = 513);

    void convertMatToPng(const std::vector<std::string>& matFileNames, 
        const std::vector<std::string>& imageFileNames,
        const std::string& inferenceDir,
        const std::string& datasetDir,
        const std::string& outputDir,
        bool grayscale = false);

private:
    static const cv::Scalar pascalColors[];

    const size_t mapSize;
    const size_t classCount;

    void convertSegmentation(const float* inference, size_t channels, 
        cv::Mat& outputImage);
    void writeSegmentedImage(const std::string& fileName, const cv::Mat& data);
};

#endif // PASCAL_PROCESSING_HPP