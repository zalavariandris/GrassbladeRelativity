

#include "AEAnimationData.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;
namespace Animation {
	namespace Translators {
		namespace AEAnimationData {
			std::vector<Animation::AnimationCurve> import(string filepath) {
				ifstream f(filepath);
				if (!f.is_open()) {
					cout << "error while opening file" << endl;
					return std::vector<Animation::AnimationCurve>();
				}

				std::vector<Animation::AnimationCurve> curves;

				std::string line;


				// Process title
				// skip to metadatablock block
				while (std::getline(f, line) && line != "") {}

				/* Process keyframes data */
				// skip to first keyframes block
				while (std::getline(f, line) && line != "") {}

				int currentBlock = 0;
				while (currentBlock < 3) {
					// process block header (simply step through)
					std::getline(f, line);

					// Process csv heading (simply step throuh)
					std::getline(f, line);

					// process csv rows one by one
					cout << "load keyframes" << endl;
					curves.push_back(Animation::AnimationCurve());
					curves.push_back(Animation::AnimationCurve());
					while (std::getline(f, line) && line != "") {
						std::string cell;
						std::stringstream linestream(line);
						int frame;
						float x, y;
						linestream >> frame >> x >> y;
						cout << "line: " << line << endl;
						cout << "  Key: " << frame << ", " << x << ", " << y << endl;
						curves[currentBlock * 2].setValueAtFrame(x, frame);
						curves[currentBlock * 2 + 1].setValueAtFrame(y, frame);
					}
					currentBlock++;
				}

				if (f.bad()) {
					cout << "error while reading file" << endl;
					return curves;
				}

				// if everythin went fine, set the curves
				return curves;
			}
		}
	}
}