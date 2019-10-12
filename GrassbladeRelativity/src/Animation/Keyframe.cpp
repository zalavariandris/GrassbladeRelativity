
#include "Keyframe.h"
#include "AnimationCurve.h"
#include <iostream>
#include <assert.h>
namespace Animation {
	void Keyframe::time(int val) {
		mTime = val;
	}

	int Keyframe::time() const {
		return mTime;
	}

	double Keyframe::value() const {
		return mValue;
	}

	void Keyframe::value(double val) {
		mValue = val;
	}

	void Keyframe::inTangent(double val) {
		mInTangent = val;
	}

	double Keyframe::inTangent() const {
		return mInTangent;
	}

	void Keyframe::outTangent(double val) {
		mOutTangent = val;
	}

	double Keyframe::outTangent() const {
		return mOutTangent;
	}
}