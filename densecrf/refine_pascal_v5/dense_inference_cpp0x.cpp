#include "dense_inference_cpp0x.hpp"


template <> enum matio_classes matio_class_map<float>() { return MAT_C_SINGLE; }
template <> enum matio_classes matio_class_map<double>() { return MAT_C_DOUBLE; }
template <> enum matio_classes matio_class_map<int>() { return MAT_C_INT32; }
template <> enum matio_classes matio_class_map<unsigned int>() { return MAT_C_UINT32; }


template <typename T>
void LoadMatFile(const std::string& fileName, T*& data, int rows, int cols, int& channels, bool do_ppm_format) {
    mat_t* matfp = Mat_Open(fileName.c_str(), MAT_ACC_RDONLY);
    if (matfp != NULL) {
        throw exception("Error opening MAT file: '" + fileName + "'.");
    }

    // Read data
    matvar_t* matvar = Mat_VarReadInfo(matfp, "data");
    if (matvar == NULL) {
        Mat_Close(matfp);
        throw exception("Field 'data' not present. In MAT file: '" + fileName + "'.");
    }

    if (matvar->class_type != matio_class_map<T>()) {
        Mat_VarFree(matvar);
        Mat_Close(matfp);
        throw exception("Field 'data' must be of the right class (single/double). In MAT file: '" + fileName + "'.");
    }
    if ((4 <= matvar->rank) && (matvar->dims[3] != 1)) {
        Mat_VarFree(matvar);
        Mat_Close(matfp);
        throw exception("Field 'data' cannot have ndims > 3. In MAT file: '" + fileName + "'.");
    }

    int fileSize = 1;
    int dataSize = rows * cols;
    for (int k = 0; k < matvar->rank; ++k) {
        fileSize *= matvar->dims[k];
        
        if (1 < k) {
            dataSize *= matvar->dims[k];
        }
    }

    assert(dataSize <= fileSize);

    T* fileData = new T[fileSize];
    data = new T[dataSize];
    
    if (Mat_VarReadDataLinear(matfp, matvar, fileData, 0, 1, fileSize) != 0) {
        delete[] data;
        delete[] fileData;
        Mat_VarFree(matvar);
        Mat_Close(matfp);
        throw exception("Error reading array 'data' from MAT file: '" + fileName + "'.");
    }

    // matvar->dims[0] : width
    // matvar->dims[1] : height
    const int channelSize = matvar->dims[0] * matvar->dims[1];
    channels = static_cast<int>(matvar->dims[2]);

    // extract from fileData
    if (do_ppm_format) {
        int out_offset = cols * channels;

        for (int channel = 0; channel < channels; ++channel) {
            for (int row = 0; row < rows; ++row) {
                for (int column = 0; column < cols; ++column) {
                    // perform transpose of fileData
                    const int in_ind    = column + row * matvar->dims[0] + channel * channelSize;    
                    const int out_ind = row * out_offset + column * channels + channel;
                    // note the minus sign
                    data[out_ind] = -fileData[in_ind]; 
                }
            }
        }
    } else {
        int out_offset = rows * cols;

        for (int channel = 0; channel < channels; ++channel) {
            for (int row = 0; row < rows; ++row) {
                for (int column = 0; column < cols; ++column) {
                    const int in_ind    = row + column * matvar->dims[0] + channel * channelSize;
                    const int out_ind = row + column * row + channel * out_offset; 
                    data[out_ind] = -fileData[in_ind];        
                }
            }
        }
    }

    Mat_VarFree(matvar);
    Mat_Close(matfp);
    delete[] fileData;
}

template <typename T>
void LoadBinFile(const std::string& fileName, T*& data, int& rows, int& cols, int& channels) {
    std::ifstream ifs(fileName.c_str(), std::ios_base::in | std::ios_base::binary);

    if (ifs.is_open() == false) {
        throw exception("Fail to open file: '" + fileName + "'.");
    }

    ifs >> rows;
    ifs >> cols;
    ifs >> channels;

    const int num_el = rows * cols * channels;
    data = new T[num_el];
    ifs.read(reinterpret_cast<char*>(data), sizeof(T) * num_el);

    ifs.close();
}

template <typename T>
void SaveBinFile(const std::string& fileName, const T* data, int rows, int cols, int channels) {
    std::ofstream ofs(fileName.c_str(), std::ios_base::out | std::ios_base::binary);

    if (ofs.is_open() == false) {
        throw exception("Fail to open file: '" + fileName + "'.");
    }    

    ofs << rows;
    ofs << cols;
    ofs << channels;

    int num_el = rows * cols * channels;
    ofs.write(reinterpret_cast<const char*>(data), sizeof(T) * num_el);

    ofs.close();
}


void ReshapeToMatlabFormat(short*& result, short* map, int rows, int cols) {
    //row-order to column-order
    for (int h = 0; h < rows; ++h) {
        for (int w = 0; w < cols; ++w) {
            const int out_index = w * rows + h;
            const int in_index    = h * cols + w;
            result[out_index] = map[in_index];
        }
    }
}


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


void listDirectory(const std::string& path, const std::string& pattern, bool stripExtension, std::vector<std::string>& fileNames) {
    DIR* dir = opendir(path.c_str());
    if (dir == NULL) {
        throw exception("Error opening dir: '" + path + "'.");
    }

    dirent* entry = readdir(dir);
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

    assert(inp.imageDir.empty() == false && inp.featureDir.empty() == false && inp.outputDir.empty() == false);
    
    std::vector<std::string> featureNamesList;
    listDirectory(inp.featureDir, ".mat", true, featureNamesList);
    
    std::vector<std::string> imageNamesList;
    generateImageNames(featureNamesList, "_blob_0", imageNamesList);


    std::string fileName;
    const bool do_ppm_format = true;

    CPrecisionTimer CTmr;
    CTmr.Start();
    for (size_t i = 0; i < featureNamesList.size(); ++i) {
        if ( (i + 1) % 100 == 0) {
            std::cout << "Processing " << i << " entry from " << featureNamesList.size() << " ..." << std::endl;
        }

        fileName = inp.imageDir + "/" + imageNamesList[i] + ".ppm";
        int featuresRows, featuresColumns;
        unsigned char* img = readPPM(fileName.c_str(), featuresColumns, featuresRows);

        fileName = inp.featureDir + "/" + featureNamesList[i] + ".mat";

        float* features = NULL;
        int featuresChannels;
        LoadMatFile(fileName, features, featuresRows, featuresColumns, featuresChannels, do_ppm_format);

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
        ReshapeToMatlabFormat(result, map, featuresRows, featuresColumns);

        // save results
        fileName = inp.outputDir + "/" + imageNamesList[i] + ".bin";
        SaveBinFile(fileName, result, featuresRows, featuresColumns, 1);
        

        delete[] result;
        delete[] features;
        delete[] img;
        delete[] map;
    }

    std::cout << "Time for inference: " << CTmr.Stop() << std::endl;

    return 0;
}
