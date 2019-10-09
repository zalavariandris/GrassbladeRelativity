
#include "Keyframe.h"
#include "AnimationCurve.h"
#include <iostream>
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
		if (mInType == Keyframe::Spline) {
			if (next() == nullptr && prev() == nullptr) {
				return 0;
			}
			const Keyframe * A = next();
			const Keyframe * B = prev();
			if (A == nullptr) { A = this; }
			if (B == nullptr) { B = this; };

			return (A->value() - B->value()) / (A->time() - B->time());
		}
		else if (mInType == Keyframe::Linear) {
			if (prev() == nullptr) {
				return 0;
			}
			return (value() - prev()->value()) / (time() - prev()->time());
		} else{
			return mInTangent;
		}
	}

	void Keyframe::outTangent(double val) {
		mOutTangent = val;
	}

	double Keyframe::outTangent() const {
		if (mOutType == Keyframe::Spline) {
			if (next() == nullptr && prev() == nullptr) {
				return 0;
			}
			const Keyframe * A = next() != nullptr ? next() : this;
			const Keyframe * B = prev() != nullptr ? prev() : this;
			return (A->value() - B->value()) / (A->time() - B->time());
		}
		else if (mOutType == Keyframe::Linear) {
			if (next() == nullptr) {
				return 0;
			}
			return (next()->value() - value()) / (next()->time() - time());
		} else {
			return mOutTangent;
		}
	}

	Keyframe * Keyframe::next() const {
		auto iNext = idx + 1;
		if (iNext < 0 || iNext >= curve->keys().size()) {
			return nullptr;
		}
		
		return &(curve->keys()[iNext]);
	}

	Keyframe * Keyframe::prev() const {
		auto iPrev = idx - 1;
		if (iPrev < 0 || iPrev >= curve->keys().size()) {
			return nullptr;
		}
		return &(curve->keys()[iPrev]);
	}
}