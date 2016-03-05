#ifndef MAT_PROCESSING_HPP
#define MAT_PROCESSING_HPP

#include <iostream>
#include <string>       

#include "exception.hpp"
#include "matio.h"


template <typename Dtype> enum matio_classes matio_class_map();
template <> enum matio_classes matio_class_map<float>() { return MAT_C_SINGLE; }
template <> enum matio_classes matio_class_map<double>() { return MAT_C_DOUBLE; }
template <> enum matio_classes matio_class_map<int>() { return MAT_C_INT32; }
template <> enum matio_classes matio_class_map<unsigned int>() { return MAT_C_UINT32; }

template <typename T>
void LoadMatFile(const std::string& fileName, T*& data, int rows, int cols,
        int& channels, bool transpose = false);

/*
 * Transforms row-ordered matrix to column-ordered.
*/
template <typename T>
void ReshapeToMatlabFormat(const T* map, int rows, int cols, T* result);



template <typename T>
void LoadMatFile(const std::string& fileName, T*& data, 
    int rows, int cols, int& channels, bool transpose) 
{
    mat_t* matfp = Mat_Open(fileName.c_str(), MAT_ACC_RDONLY);
    if (matfp == NULL) {
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
        throw exception("Field 'data' must be of the" 
            "right class (single/double). In MAT file: '" + fileName + "'.");
    }
    if ((4 <= matvar->rank) && (matvar->dims[3] != 1)) {
        Mat_VarFree(matvar);
        Mat_Close(matfp);
        throw exception("Field 'data' cannot have"
            " ndims > 3. In MAT file: '" + fileName + "'.");
    }

    int fileSize = 1;
    int dataSize = rows * cols;
    for (int k = 0; k < matvar->rank; ++k) {
        fileSize *= matvar->dims[k];
        
        if (1 < k) {
            dataSize *= matvar->dims[k];
        }
    }

    if (fileSize < dataSize) {
        Mat_VarFree(matvar);
        Mat_Close(matfp);     
        throw exception("Data size have to be <= fileSize.");
    }

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
    channels = matvar->dims[2];

    // extract from fileData and crop mat file to image size (cols, rows)
    if (transpose == true) {
        const int out_offset = cols * channels;

        for (int channel = 0; channel < channels; ++channel) {
            for (int row = 0; row < rows; ++row) {
                for (int column = 0; column < cols; ++column) {
                    // fileData[channel][row][column] -> data[row][column][channel]

                    // perform transpose of fileData
                    const int in_ind  = column  + row * matvar->dims[0] + channel * channelSize;
                    const int out_ind = channel + column * channels     + row * out_offset;                                        
                    data[out_ind] = -fileData[in_ind]; // note the minus sign
                }
            }
        }
    } else {
        for (int channel = 0; channel < channels; ++channel) {
            for (int row = 0; row < rows; ++row) {
                for (int column = 0; column < cols; ++column) {
                    // fileData[channel][column][row] -> data[channel][column][row]

                    const int in_ind  = row + column * matvar->dims[0] + channel * channelSize;
                    const int out_ind = row + column * rows            + channel * channelSize; 
                    data[out_ind] = -fileData[in_ind]; // note the minus sign
                }
            }
        }
    }

    Mat_VarFree(matvar);
    Mat_Close(matfp);
    delete[] fileData;
}

template <typename T>
void ReshapeToMatlabFormat(const T* map, int rows, int cols, T* result) {
    for (int h = 0; h < rows; ++h) {
        for (int w = 0; w < cols; ++w) {
            const int out_index = w * rows + h;
            const int in_index  = h * cols + w;
            result[out_index] = map[in_index];
        }
    }
}

#endif // MAT_PROCESSING_HPP