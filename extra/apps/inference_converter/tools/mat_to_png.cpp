#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>

#include "exception.hpp"
#include "mat_processing.hpp"
#include "find_files.hpp"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


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

    const size_t netReceptiveFieldSize = 513;
    const size_t pascalClassCount = 21;

    //preallocate memory for images
    const size_t inferenceDataSize = netReceptiveFieldSize * netReceptiveFieldSize * pascalClassCount;
    float* inference = new float[inferenceDataSize];

    const size_t matBufferSize = netReceptiveFieldSize * netReceptiveFieldSize * pascalClassCount * sizeof(float);
    void* matBuffer = malloc(matBufferSize);

    cv::Mat outputImage(netReceptiveFieldSize, netReceptiveFieldSize, CV_8UC3);
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
        outputImage.create(datasetImage.rows, datasetImage.cols, CV_8UC3);
        
        int channels = 0;
        LoadMatFile(inputDir + "/" + matFilesList[i] + ".mat", 
                    inference, inferenceDataSize, datasetImage.rows, datasetImage.cols,
                    channels, true, matBuffer, matBufferSize
                   );

	const size_t partSize = 16;
        const size_t yparts = outputImage.rows / partSize;
        const size_t xparts = outputImage.cols / partSize;	
	for (int yp = 0; yp < yparts; ++yp) {
            for (int xp = 0; xp < xparts; ++xp) {
		for (int y_ = 0; y_ < partSize ; ++y_) {
		    for (int x_ = 0; x_ < partSize ; ++x_) {
			const size_t x = x_ + xp * partSize;
			const size_t y = y_ + yp * partSize;

			const auto inferenceBegin = inference + (y * datasetImage.cols + x) * channels;
			const auto maxChannel = std::max_element(inferenceBegin, inferenceBegin + channels);
        	        const size_t colorIndex = std::distance(inferenceBegin, maxChannel);
         	        auto point = outputImage(cv::Range(y, y + 1), cv::Range(x, x + 1));		
			point.data[0] = cv::saturate_cast<uchar>(255.0 * pascalColors[colorIndex](2));
			point.data[1] = cv::saturate_cast<uchar>(255.0 * pascalColors[colorIndex](1));
			point.data[2] = cv::saturate_cast<uchar>(255.0 * pascalColors[colorIndex](0));
		    }
		}
            }
        }
	for (int y = yparts * partSize; y < outputImage.rows; ++y) {
	    for (int x = xparts * partSize; x < outputImage.cols; ++x) {
		const auto inferenceBegin = inference + (y * datasetImage.cols + x) * channels;
		const auto maxChannel = std::max_element(inferenceBegin, inferenceBegin + channels);
        	const size_t colorIndex = std::distance(inferenceBegin, maxChannel);
         	auto point = outputImage(cv::Range(y, y + 1), cv::Range(x, x + 1));		
		point.data[0] = cv::saturate_cast<uchar>(255.0 * pascalColors[colorIndex](2));
		point.data[1] = cv::saturate_cast<uchar>(255.0 * pascalColors[colorIndex](1));
		point.data[2] = cv::saturate_cast<uchar>(255.0 * pascalColors[colorIndex](0));		
	    }
	}

        
        cv::imwrite(outputDir + "/" + imagesList[i] + ".png", outputImage);
    }

    free(matBuffer);
    delete[] inference;

    std::cout << "Done." << std::endl;

    return 0;
}