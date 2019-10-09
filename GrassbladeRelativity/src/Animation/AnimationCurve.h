#pragma once

#include <vector>
#include "Keyframe.h"
#include <iostream>
#include <string>
namespace Animation {
	class AnimationCurve {
	public:
		AnimationCurve::AnimationCurve() {};
		AnimationCurve(std::vector<Keyframe> _keys) :mKeys(_keys) {
			sortKeys();
		};

		void sortKeys();

		std::string label;

		std::vector<Keyframe> & keys();

		double evaluate(double t, Keyframe keyframe0, Keyframe keyframe1) const;

		void setKeys(std::vector<Keyframe> val);

		void setValueAtFrame(double value, int frame);
		double getValueAtFrame(int time) const;
	private:
		std::vector<Keyframe> mKeys;
	};
}