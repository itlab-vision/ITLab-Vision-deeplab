#ifndef __MOVIE_MAKER_HPP__
#define __MOVIE_MAKER_HPP__

#include <iostream>
#include <fstream>

#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\video\video.hpp"

class MovieMaker
{
public:
    MovieMaker(int frameWidth, int frameHeight,
               int frameRepeat = 1, int fps = 30) :
        frameWidth(frameWidth), frameHeight(frameHeight),
        frameRepeat(frameRepeat), fps(fps)
    {}

    void createVideo(std::istream& list, const std::string& outputFileName);
    void createVideo(std::vector<std::vector<std::string> > &imagesSet,
            const std::string& outputFileName);
private:
    int frameWidth;
    int frameHeight;
    int frameRepeat;
    int fps;

    void mergeImages(const std::vector<cv::Mat> &images, cv::Mat &frame);
    bool checkSizeImagesSet(const std::vector<std::vector<std::string> > &imagesSet);
    void createVOCLegend(cv::Mat &legend);
    void preprocessImage(cv::Mat& image);
};

#endif
