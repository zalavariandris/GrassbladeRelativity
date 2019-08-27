#pragma once
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <string>
#include "ofImage.h"
#include <iostream>
class Reader {
private:
	cv::VideoCapture _cap;
	cv::Mat buffer;

	bool init(std::string file) {
		return _cap.open(file);
	}

	void destruct() {
		_cap.release();
	}

	uchar* getDataAtFrame(int frame) {
		if (!validateFrame(frame)) {
			return nullptr;
		}
		else if (frame == getCurrentFrame()) {
			return buffer.data;
		}
		else if (frame == getCurrentFrame()+1) {
			// play next frame
			std::cout << "play" << std::endl;
			_cap.read(buffer);
			return buffer.data;
		}
		else {
			// seek to specified frame
			std::cout << "seek" << std::endl;
			_cap.set(cv::CAP_PROP_POS_FRAMES, frame);
			_cap >> buffer;
			return buffer.data;
		}
	}

	uchar * getDataNextFrame() {
		_cap.read(buffer);
		return buffer.data;
	}

private:
	std::string _file;

public:
	int getFrameCount() const {
		return _cap.get(cv::CAP_PROP_FRAME_COUNT);
	}

	int getWidth() {
		return _cap.get(cv::CAP_PROP_FRAME_WIDTH);
	}

	int getHeight() {
		return _cap.get(cv::CAP_PROP_FRAME_HEIGHT);
	}

	int getCurrentFrame() {
		// from opencv documentation: 
		//   0-based index of the frame to be decoded/captured next.
		// so it get the current frame, need to subtract 1
		return _cap.get(cv::CAP_PROP_POS_FRAMES)-1;
	}

	bool validateFrame(int frame) const {
		return frame >= 0 && frame < getFrameCount();
	}

	bool setFile(std::string file) {
		if (init(file)) {
			_file = file;
			return true;
		}
		return false;
	}

	std::string getFile() {
		return _file;
	}

	Reader(std::string file, bool useCache) {
		setFile(file);
	}

	~Reader() {
		destruct();
	}

	ofImage getImageAtFrame(int frame) {
		if (!validateFrame(frame))
			return ofImage();

		ofImage img;
		img.setFromPixels(getDataAtFrame(frame), getWidth(), getHeight(), OF_IMAGE_COLOR, false);
		return img;
	}
};
