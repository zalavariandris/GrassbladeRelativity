#pragma once
#include "AnimationCurve.h"
#include <vector>
#include "Keyframe.h"
#include <algorithm> //sort

namespace Animation {
	double AnimationCurve::evaluate(double t, Keyframe keyframe0, Keyframe keyframe1) const
	{
		float dt = keyframe1.time() - keyframe0.time();

		float m0 = keyframe0.outTangent() * dt;
		float m1 = keyframe1.inTangent() * dt;

		float t2 = t * t;
		float t3 = t2 * t;

		float a = 2 * t3 - 3 * t2 + 1;
		float b = t3 - 2 * t2 + t;
		float c = t3 - t2;
		float d = -2 * t3 + 3 * t2;

		return a * keyframe0.value() + b * m0 + c * m1 + d * keyframe1.value();
	}

	void AnimationCurve::setKeys(std::vector<Keyframe> val) {
		mKeys = val;
		sortKeys();
	}


	std::vector<Keyframe> & AnimationCurve::keys(){
		return mKeys;
	}

	void AnimationCurve::setValueAtFrame(double value, int frame) {
		if (mKeys.empty())
			mKeys.push_back(Keyframe(frame, value));

		if (frame < mKeys[0].time())
			mKeys.insert(mKeys.begin(), 1, Keyframe(frame, value));

		if (frame > mKeys[mKeys.size() - 1].time())
			mKeys.push_back(Keyframe(frame, value));

		// find key at frame
		auto i = -1;
		for (int i = 0; i < mKeys.size(); i++)
			if (mKeys[i].time() == frame) {
				mKeys[i].value(value);
				return;
			}

		// find embace keys
		int i1 = 0;
		while (mKeys[i1].time() <= frame)
			i1++;
		int i0 = i1 - 1;

		mKeys.insert(mKeys.begin() + i1, 1, Keyframe(frame, value));
		sortKeys();
	}

	void AnimationCurve::sortKeys() {
		std::sort(mKeys.begin(), mKeys.end(), [](auto A, auto B) {
			return A.time() < B.time();
		});
		for (auto i = 0; i < mKeys.size(); i++) {
			mKeys[i].idx = i;
			mKeys[i].curve = this;
		}
	}

	double AnimationCurve::getValueAtFrame(int time) const{

		// find the keys
		if (mKeys.empty()) {
			return 0;
		}

		if (time < mKeys[0].time()) {
			return mKeys[0].value();
		}

		if (time > mKeys[mKeys.size() - 1].time()) {
			return mKeys[mKeys.size() - 1].value();
		}

		// search for a key that exactly matches the queried time
		for (int i = 0; i < mKeys.size(); i++)
			if (mKeys[i].time() == time) {
				return mKeys[i].value();
			}

		// find embace keys
		int i1 = 0;
		while (mKeys[i1].time() < time) {
			i1++;
		}
		int i0 = i1 - 1;

		// normalize t
		double t = (double)(time - mKeys[i0].time()) / (mKeys[i1].time() - mKeys[i0].time());
		double value = evaluate(t, mKeys[i0], mKeys[i1]);

		return value;
	}
}