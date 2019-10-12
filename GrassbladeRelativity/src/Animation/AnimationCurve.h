#pragma once

#include <vector>
#include "Keyframe.h"
#include <iostream>
#include <string>
namespace Animation {
	class AnimationCurve {
	public:
		AnimationCurve::AnimationCurve() {};
		AnimationCurve(std::vector<Keyframe> _keys) :mKeys(_keys) {};

		std::string label{"-curve-"};

		std::vector<Keyframe> & keys();

		void setKeys(std::vector<Keyframe> val);

		void setValueAtFrame(double value, int frame);

		double getValueAtFrame(double time) const;

	private:
		std::vector<Keyframe> mKeys;
	};
}