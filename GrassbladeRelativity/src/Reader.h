#pragma once
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp> // for: color conversion
#include <opencv2/imgproc/types_c.h> // for: CV_BGR2RGBA

#include <string>
#include "ofImage.h"
#include <iostream>
#include <stdexcept>
#include <map>
class Reader {
public:
	Reader();
	Reader(std::string file);
	~Reader();

	int getFrameCount() const;
	int getFirstFrame() const;
	int getLastFrame() const;
	int getWidth();
	int getHeight();
	int getCurrentFrame();
	bool validateFrame(int frame) const;
	bool setFile(std::string filepath);
	std::string getFile() const;

	cv::Mat getMatAtFrame(int frame);
	ofImage getImageAtFrame(int frame);

private:
	std::string _filepath;
	cv::VideoCapture _cap;
	cv::Mat buffer;
	std::map<int, ofImage> imageCache;
};
