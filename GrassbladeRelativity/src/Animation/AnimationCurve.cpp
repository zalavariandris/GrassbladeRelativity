#pragma once
#include "AnimationCurve.h"
#include <vector>
#include "Keyframe.h"
#include <algorithm> //sort

namespace Animation {
	void AnimationCurve::setKeys(std::vector<Keyframe> val) {
		mKeys = val;
	}


	std::vector<Keyframe> & AnimationCurve::keys(){
		return mKeys;
	}

	void AnimationCurve::setValueAtFrame(double value, int frame) {
		if (mKeys.empty()) {
			mKeys.push_back(Keyframe(frame, value));
			return;
		}
		
		if (frame < mKeys[0].time()) {
			mKeys.insert(mKeys.begin(), 1, Keyframe(frame, value));
			return;
		}

		if (frame > mKeys[mKeys.size() - 1].time()) {
			mKeys.push_back(Keyframe(frame, value));
			return;
		}

		// find key at frame
		auto i = -1;
		for (int i = 0; i < mKeys.size(); i++)
			if (mKeys[i].time() == frame) {
				mKeys[i].value(value);
				return;
			}

		// find embace keys
		int i1 = 0;
		while (i1 < mKeys.size() && mKeys[i1].time() <= frame)
			i1++;
		int i0 = i1 - 1;

		mKeys.insert(mKeys.begin() + i1, 1, Keyframe(frame, value));
	}

	double AnimationCurve::getValueAtFrame(double time) const{

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
		while (mKeys[i1].time() <= time) {
			i1++;
		}
		int i0 = i1 - 1;


		// calculate tangent based on key type
		auto evalLinearOutTangentAtIndex = [this](int idx)->double {
			bool isLast = idx == mKeys.size() - 1;
			if (isLast) {
				return 0;
			}

			double x = mKeys[idx + 1].time() - mKeys[idx].time();
			double y = mKeys[idx + 1].value() - mKeys[idx].value();
			return y / x;
		};

		auto evalLinearInTangentAtIndex = [this](int idx)->double {
			bool isFirst = idx == 0;
			if (isFirst) {
				return 0;
			}

			double x = mKeys[idx].time() - mKeys[idx-1].time();
			double y = mKeys[idx].value() - mKeys[idx-1].value();
			return y / x;
		};

		auto evalSplineOutTangentAtIndex = [this](int idx)->double {
			bool isFirst = idx == 0;
			bool isLast = idx == mKeys.size() - 1;
			if (isLast) {
				return 0;
			}
			if(isFirst) {
				double x = mKeys[idx+1].time() - mKeys[idx].time();
				double y = mKeys[idx+1].value() - mKeys[idx].value();
				return y / x;
			}
			double x = mKeys[idx + 1].time() - mKeys[idx - 1].time();
			double y = mKeys[idx + 1].value() - mKeys[idx - 1].value();
			return y / x;
		};

		auto evalSplineInTangentAtIndex = [this](int idx)->double {
			bool isFirst = idx == 0;
			bool isLast = idx == mKeys.size() - 1;
			if (isFirst) {
				return 0;
			}
			if(isLast) {
				double x = mKeys[idx].time() - mKeys[idx-1].time();
				double y = mKeys[idx].value() - mKeys[idx-1].value();
				return y / x;
			}
			double x = mKeys[idx + 1].time() - mKeys[idx - 1].time();
			double y = mKeys[idx + 1].value() - mKeys[idx - 1].value();
			return y / x;
		};

		// EVALUATE
		double dt = mKeys[i1].time() - mKeys[i0].time();
		double t = (double)(time - mKeys[i0].time()) / dt; // [0,1]

		double m0 = evalSplineOutTangentAtIndex(i0) * dt;
		double m1 = evalSplineInTangentAtIndex(i1) * dt;

		double t2 = t * t;
		double t3 = t2 * t;
		double a = 2 * t3 - 3 * t2 + 1;
		double b = t3 - 2 * t2 + t;
		double c = t3 - t2;
		double d = -2 * t3 + 3 * t2;

		return a * mKeys[i0].value() + b * m0 + c * m1 + d * mKeys[i1].value();
	}
}