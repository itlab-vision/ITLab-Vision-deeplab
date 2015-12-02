/*
 * The code is modified from the NIPS demo code by Philipp Krahenbuhl
*/
/*
        Copyright (c) 2011, Philipp Krahenbuhl
        All rights reserved.

        Redistribution and use in source and binary forms, with or without
        modification, are permitted provided that the following conditions are met:
                * Redistributions of source code must retain the above copyright
                notice, this list of conditions and the following disclaimer.
                * Redistributions in binary form must reproduce the above copyright
                notice, this list of conditions and the following disclaimer in the
                documentation and/or other materials provided with the distribution.
                * Neither the name of the Stanford University nor the
                names of its contributors may be used to endorse or promote products
                derived from this software without specific prior written permission.

        THIS SOFTWARE IS PROVIDED BY Philipp Krahenbuhl ''AS IS'' AND ANY
        EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
        WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
        DISCLAIMED. IN NO EVENT SHALL Philipp Krahenbuhl BE LIABLE FOR ANY
        DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
        (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
        LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
        ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
        (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
        SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <ctime>
#include <fstream>    
#include <utility>
#include <vector>

#include <dirent.h>
#include <fnmatch.h>

#include "densecrf.h"
#include "exception.hpp"
#include "bin_processing.hpp"
#include "mat_processing.hpp"
        
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


class InputData {
public:
    std::string imageDir;
    std::string featureDir;
    std::string outputDir;
    int maxIterations;
    float posW;
    float posXStd;
    float posYStd;
    float bilateralW;
    float bilateralXStd;
    float bilateralYStd;
    float bilateralRStd;
    float bilateralGStd;
    float bilateralBStd;

    InputData(int argc, char** argv);

private:
    void init();
};

std::ostream& operator<<(std::ostream& os, const InputData& data);

std::ostream& operator<<(std::ostream& os, const InputData& inp) {
    return os
        << "Input Parameters: " << std::endl
        << "imageDir:                     " << inp.imageDir << std::endl
        << "featureDir:             " << inp.featureDir << std::endl
        << "outputDir:                    " << inp.outputDir << std::endl
        << "MaxIterations:        " << inp.maxIterations << std::endl
    //    << "MaxImgSize:             " << inp.MaxImgSize << std::endl
    //    << "NumClass:                 " << inp.NumClass << std::endl
        << "posW:            " << inp.posW << std::endl
        << "posXStd:     " << inp.posXStd << std::endl
        << "posYStd:     " << inp.posYStd << std::endl
        << "Bi_W:        " << inp.bilateralW << std::endl
        << "Bi_X_Std:    " << inp.bilateralXStd << std::endl
        << "Bi_Y_Std:    " << inp.bilateralYStd << std::endl
        << "Bi_R_Std:    " << inp.bilateralRStd << std::endl
        << "Bi_G_Std:    " << inp.bilateralGStd << std::endl
        << "Bi_B_Std:    " << inp.bilateralBStd << std::endl;
}

InputData::InputData(int argc, char** argv)   
{
    init();
    for(int k = 1; k < argc; ++k) {
        if (std::strcmp(argv[k], "-id") == 0 && k + 1 != argc) {
            imageDir = argv[++k];
        } else if (std::strcmp(argv[k], "-fd")==0 && k+1!=argc) {
            featureDir = argv[++k];
        } else if (std::strcmp(argv[k], "-sd")==0 && k+1!=argc) {
            outputDir = argv[++k];
        } else if (std::strcmp(argv[k], "-i")==0 && k+1!=argc) {
            maxIterations = atoi(argv[++k]);
        } else if (std::strcmp(argv[k], "-px")==0 && k+1!=argc) {
            posXStd = atof(argv[++k]);
        } else if (std::strcmp(argv[k], "-py")==0 && k+1!=argc) {
            posYStd = atof(argv[++k]);
        } else if (std::strcmp(argv[k], "-pw")==0 && k+1!=argc) {
            posW = atof(argv[++k]);
        } else if (std::strcmp(argv[k], "-bx")==0 && k+1!=argc) {
            bilateralXStd = atof(argv[++k]);
        } else if (std::strcmp(argv[k], "-by")==0 && k+1!=argc) {
            bilateralYStd = atof(argv[++k]);
        } else if (std::strcmp(argv[k], "-bw")==0 && k+1!=argc) {
            bilateralW = atof(argv[++k]);
        } else if (std::strcmp(argv[k], "-br")==0 && k+1!=argc) {
            bilateralRStd = atof(argv[++k]);
        } else if (std::strcmp(argv[k], "-bg")==0 && k+1!=argc) {
            bilateralGStd = atof(argv[++k]);
        } else if (std::strcmp(argv[k], "-bb")==0 && k+1!=argc) {
            bilateralBStd = atof(argv[++k]);
        } 
    }
}

void InputData::init() {
    maxIterations = 10;

    posW    = 3;
    posXStd = 3;
    posYStd = 3;
    bilateralW    = 5;
    bilateralXStd = 70;
    bilateralYStd = 70;
    bilateralRStd = 5;
    bilateralGStd = 5;
    bilateralBStd = 5;
}

/*
 * Loads an image and coverts it to RGB(888) pixel format.
 * Returns false on fail, true on success.
*/
bool loadImage(const std::string& fileName, cv::Mat& image) {
    image = cv::imread(fileName);
    if (image.total() == 0) {
        return false;
    }
    //convert to RGB pixel format
    switch (image.type()) {
    case CV_8UC3: 
        cv::cvtColor(image, image, CV_BGR2RGB);
        break;
    case CV_8UC4:
        cv::cvtColor(image, image, CV_BGRA2RGB);
        break;
    default: { /*none*/ }
    }
    return true;
}

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

int main(int argc, char* argv[]) {
    InputData inp(argc, argv);
    std::cout << inp;
    if (inp.imageDir.empty() == true) {
        std::cerr << "Images directory not set. Exiting.";
        return 1; // magic
    }
    if (inp.featureDir.empty() == true) {
        std::cerr << "Features directory not set. Exiting.";
        return 2; // magic      
    }
    if (inp.outputDir.empty() == true) {
        std::cerr << "Output directory not set. Exiting.";
        return 3; // magic
    }
    
    std::vector<std::string> featureNamesList;
    listDirectory(inp.featureDir, "*.mat", true, featureNamesList);
    
    std::vector<std::string> imageNamesList;
    generateImageNames(featureNamesList, "_blob_0", imageNamesList);

    std::string fileName;
    cv::Mat loadedImage;

    const clock_t timeStart = clock();
    for (size_t i = 0; i < featureNamesList.size(); ++i) {
        if (i % 100 == 0) {
            std::cout << "Processing " << i << " entry from " << featureNamesList.size() << " ..." << std::endl;
        }

        fileName = inp.imageDir + "/" + imageNamesList[i] + ".jpg";
        
        if (loadImage(fileName, loadedImage) == false) {
            std::cerr << "Failed to open image: '" << fileName << "'. Skipping." << std::endl;
            continue;
        }
        int featuresRows = loadedImage.rows;
        int featuresColumns = loadedImage.cols;
        unsigned char* img = loadedImage.data;

        fileName = inp.featureDir + "/" + featureNamesList[i] + ".mat";

        /* CRF works with the next image data order: data[height][width][channels] 
         * so we have to transpose loaded mat file to row-order
        */
        const bool transpose = true;
        float* features = NULL;
        int featuresChannels;
        LoadMatFile(fileName, features, featuresRows, featuresColumns, featuresChannels, transpose);

        // Setup the CRF model
        DenseCRF2D crf(featuresColumns, featuresRows, featuresChannels);
        // Specify the unary potential as an array of size W*H*(#classes)
        // packing order: x0y0l0 x0y0l1 x0y0l2 .. x1y0l0 x1y0l1 ... (row-order)
        crf.setUnaryEnergy(features);
        // add a color independent term (feature = pixel location 0..W-1, 0..H-1)
        crf.addPairwiseGaussian(inp.posXStd, inp.posYStd, inp.posW);

        // add a color dependent term (feature = xyrgb)
        crf.addPairwiseBilateral(inp.bilateralXStd, inp.bilateralYStd, inp.bilateralRStd, inp.bilateralGStd, inp.bilateralBStd, img, inp.bilateralW);
    
        // Do map inference
        short* map = new short[featuresRows * featuresColumns];
        crf.map(inp.maxIterations, map);

        short* result = new short[featuresRows * featuresColumns];
        ReshapeToMatlabFormat(map, featuresRows, featuresColumns, result);

        // save results
        fileName = inp.outputDir + "/" + imageNamesList[i] + ".bin";
        SaveBinFile(fileName, result, featuresRows, featuresColumns, 1);
        
        delete[] result;
        delete[] features;
        delete[] map;
    }

    std::cout << "Time for inference: " << (double(clock() - timeStart) / CLOCKS_PER_SEC) << std::endl;

    return 0;
}