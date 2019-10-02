#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include "../AnimCurve.h"

namespace Animation {
	namespace Translators {
		namespace MayaAnim {
			std::vector<Animation::AnimCurve> import(std::string filepath);
		}
	}
}


