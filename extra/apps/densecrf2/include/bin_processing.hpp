#ifndef BIN_PROCESSING_HPP
#define BIN_PROCESSING_HPP

#include <fstream>
#include <ios>
#include <iostream>
#include <string>
#include <stdexcept>


template <typename T>
void LoadBinFile(const std::string& fileName, T* data, int& rows, int& cols, int& channels);

template <typename T>
void SaveBinFile(const std::string& fileName, const T* data, int rows, int cols, int channels);


template <typename T>
void LoadBinFile(const std::string& fileName, T* data, int& rows, int& cols, int& channels) {
    std::ifstream ifs(fileName.c_str(), std::ios_base::in | std::ios_base::binary);

    if (ifs.is_open() == false) {
        throw std::runtime_error("Fail to open file: '" + fileName + "'.");
    }

    ifs.read(reinterpret_cast<char*>(&rows), sizeof(int));
    ifs.read(reinterpret_cast<char*>(&cols), sizeof(int));
    ifs.read(reinterpret_cast<char*>(&channels), sizeof(int));

    const int num_el = rows * cols * channels;
    data = new T[num_el];
    ifs.read(reinterpret_cast<char*>(data), sizeof(T) * num_el);

    ifs.close();
}

template <typename T>
void SaveBinFile(const std::string& fileName, const T* data, int rows, int cols, int channels) {
    std::ofstream ofs(fileName.c_str(), std::ios_base::out | std::ios_base::binary);

    if (ofs.is_open() == false) {
        throw std::runtime_error("Fail to open file: '" + fileName + "'.");
    }

    ofs.write(reinterpret_cast<const char*>(&rows), sizeof(int));
    ofs.write(reinterpret_cast<const char*>(&cols), sizeof(int));
    ofs.write(reinterpret_cast<const char*>(&channels), sizeof(int));

    const int num_el = rows * cols * channels;
    ofs.write(reinterpret_cast<const char*>(data), sizeof(T) * num_el);

    ofs.close();
}


#endif // BIN_PROCESSING_HPP