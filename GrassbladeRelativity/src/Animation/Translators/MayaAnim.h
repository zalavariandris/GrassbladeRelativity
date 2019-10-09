#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include "../AnimationCurve.h"

namespace Animation {
	namespace Translators {
		namespace MayaAnim {
			std::vector<Animation::AnimationCurve> import(std::string filepath);
		}
	}
}


