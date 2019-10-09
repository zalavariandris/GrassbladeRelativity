#pragma once
#include "../AnimationCurve.h"
#include <istream>
namespace Animation {
	namespace Translators {
		namespace AEAnimationData {
			std::vector<Animation::AnimationCurve> import(std::string filepath);
		}
	}
}