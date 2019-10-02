#pragma once
#include "../AnimCurve.h"
#include <istream>
namespace Animation {
	namespace Translators {
		namespace AEAnimationData {
			std::vector<Animation::AnimCurve> import(std::string filepath);
		}
	}
}