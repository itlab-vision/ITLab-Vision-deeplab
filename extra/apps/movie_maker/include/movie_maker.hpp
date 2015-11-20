#ifndef __MOVIE_MAKER_H__
#define __MOVIE_MAKER_H__

#include <iostream>
#include <fstream>

#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\video\video.hpp"

class MovieMaker {
public:
	MovieMaker(int frameWidth, int frameHeight, int frameRepeat = 1, int fps = 30) :
		frameWidth(frameWidth), frameHeight(frameHeight), frameRepeat(frameRepeat), fps(fps)
	{}

	void createVideo(std::istream& list, const std::string& outputFile);
private:
	int frameWidth;
	int frameHeight;
	int frameRepeat;
	int fps;

	void preprocessImage(cv::Mat& image);
};

#endif
