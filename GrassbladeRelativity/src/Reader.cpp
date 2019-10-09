#include "Reader.h"

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

Reader::Reader() {}

Reader::Reader(std::string file) {
	setFile(file);
}

Reader::~Reader() {
	_cap.release();
}

bool Reader::setFile(std::string filepath) {
	_filepath = filepath;
	if (_cap.open(filepath)) {
		return true;
	}
	else {
		_cap.open(ofToDataPath("NoSignal.mov"));
		return false;
	}
}

cv::Mat Reader::getMatAtFrame(int frame) {
	if (!validateFrame(frame)) {
		return cv::Mat();
	}


	if (frame == getCurrentFrame()) {
		return buffer;
	}
	else if (frame == getCurrentFrame() + 1) {
		// play next frame
		_cap.read(buffer);
		return buffer;
	}
	else {
		// seek to specified frame
		_cap.set(cv::CAP_PROP_POS_FRAMES, frame);

		// read and convert to rgb
		_cap >> buffer;

		return buffer;
	}
}

int Reader::getFrameCount() const {
	return _cap.get(cv::CAP_PROP_FRAME_COUNT);
}

int Reader::getFirstFrame() const {
	// if this is a movie file, than the first frame is always 0
	return 0;
}

int Reader::getLastFrame() const {
	// if this is a movie file, than the last frame is the frames count-1
	return getFrameCount() - 1;
}

int Reader::getWidth() {
	return _cap.get(cv::CAP_PROP_FRAME_WIDTH);
}

int Reader::getHeight() {
	return _cap.get(cv::CAP_PROP_FRAME_HEIGHT);
}

int Reader::getCurrentFrame() {
	// from opencv documentation: 
	//   0-based index of the frame to be decoded/captured next.
	// so it get the current frame, need to subtract 1
	return _cap.get(cv::CAP_PROP_POS_FRAMES) - 1;
}

bool Reader::validateFrame(int frame) const {
	return frame >= 0 && frame < getFrameCount();
}

std::string Reader::getFile() const {
	return _filepath;
}

ofImage Reader::getImageAtFrame(int frame) {
	if (!validateFrame(frame))
		return ofImage();

	if (imageCache.find(frame) != imageCache.end())
		return imageCache[frame];

	// load to of image
	ofImage img;

	cv::Mat rgb;
	cv::Mat mat = getMatAtFrame(frame);
	cv::cvtColor(buffer, rgb, CV_BGR2RGB);

	img.setFromPixels(rgb.data, getWidth(), getHeight(), OF_IMAGE_COLOR, false);
	img.getTexture().setTextureWrap(GL_REPEAT, GL_REPEAT);
	imageCache.clear();
	imageCache[frame] = img;
	return img;
}
