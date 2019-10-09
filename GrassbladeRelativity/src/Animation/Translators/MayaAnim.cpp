#include "MayaAnim.h"

using namespace std;

namespace {
	bool startswith(std::string text, std::string prefix) {
		return text.substr(0, prefix.length()) == prefix;
	}
}

namespace Animation {
	namespace Translators {
		namespace MayaAnim {
			std::vector<Animation::AnimationCurve> import(std::string filepath) {
				/*
				create AnimCurves from maya .anim file
				see maya file formats: https://courses.cs.washington.edu/courses/cse459/06wi/help/mayaguide/Reference/FileFormats.pdf
				*/
				std::vector<Animation::AnimationCurve> curves;

				// load file
				std::ifstream file(filepath);

				// parse multiple anim data
				std::string line;

				while (getline(file, line)) {
					// parse file body

					// parse anim curve
					if (startswith(line, "anim ")) {
						stringstream linestream(line);
						string anim, full_attribute_name, leaf_attribute_name, node_name;
						linestream >> anim >> full_attribute_name >> leaf_attribute_name >> node_name;

						while (getline(file, line)) {
							if (line == "  keys {") { // begin keys
								Animation::AnimationCurve curve;

								curve.label = node_name + "." + leaf_attribute_name;
								while (getline(file, line)) {
									if (line == "  }") {
										break;
									}
									// parse key
									stringstream linestream{ line };
									int frame;
									double value;
									linestream >> frame >> value;
									curve.setValueAtFrame(value, frame);
								}
								curves.push_back(curve);
								break;
							}
						}
					}
				}
				std::cout << "imported " << curves.size() << "curves: ";
				for (auto & curve : curves) {
					std::cout << "- " << curve.label << " with " << curve.keys().size() << " keys";
				}
				return curves;
			}
		}

	}
}