/*
 * The code is modified from the NIPS demo code by Philipp Krähenbühl
 *
 * Added: Support LoadMatFile and SaveMatFile. 
 *
 *
 */
/*
        Copyright (c) 2011, Philipp Krähenbühl
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

        THIS SOFTWARE IS PROVIDED BY Philipp Krähenbühl ''AS IS'' AND ANY
        EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
        WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
        DISCLAIMED. IN NO EVENT SHALL Philipp Krähenbühl BE LIABLE FOR ANY
        DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
        (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
        LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
        ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
        (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
        SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <cstdio>
#include <iostream>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <fstream>
#include <utility>
#include <exception>

#include <dirent.h>
#include <fnmatch.h>

#include "matio.h"

#include "../libDenseCRF/densecrf.h"
#include "../libDenseCRF/util.h"
#include "../util/Timer.h"


class exception : public std::exception {
public:
    exception(const std::string& message) :
        message(message)
    {}

    exception(std::string&& message) :
        message(std::move(message))
    {}

    virtual ~exception() throw() {}

    virtual const char* what() const throw() {
        return message.c_str();
    }
private:
    std::string message;
};


template <typename Dtype> enum matio_classes matio_class_map();


template <typename T>
void LoadMatFile(const std::string& fileName, T*& data, int rows, int cols,
        int& channels , bool do_ppm_format = false);

template <typename T>
void LoadBinFile(const std::string& fileName, T*& data, int& rows, int& cols, int& channels);

template <typename T>
void SaveBinFile(const std::string& fileName, const T* data, int rows, int cols, int channels);



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
