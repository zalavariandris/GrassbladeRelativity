#pragma once

namespace Animation {
	class AnimationCurve;
	class Keyframe {
	public:
		enum TangentType {
			Fixed,
			Spline,
			Linear
		};
		Keyframe() {};
		Keyframe(double time, double value) :
			mTime(time), mValue(value) {};

		Keyframe(double time, double value, TangentType type) :
			mTime(time), mValue(value), mInType(type), mOutType(type) {};

		Keyframe(double time, double value, double inTangent, double outTangent) :
			mTime(time), mValue(value), mInType(Fixed), mOutType(Fixed), mInTangent(inTangent), mOutTangent(outTangent) {};

		bool selected{ false };

		void time(int val);

		int time() const;

		double value() const;

		void value(double val);

		void inTangent(double val);

		double inTangent() const;

		void outTangent(double val);

		double outTangent() const;

		Keyframe * next() const;

		Keyframe * prev() const;

		TangentType inType() {
			return mInType;
		}

		void inType(TangentType val) {
			mInType = val;
		}

		TangentType outType() {
			return mOutType;
		}

		void outType(TangentType val) {
			mOutType = val;
		}

	private:
		int mTime{ 0 };
		double mValue{ 0 };
		TangentType mInType{ Spline };
		TangentType mOutType{ Spline };
		double mInTangent{0};
		double mOutTangent{0};
		friend class AnimationCurve;
		AnimationCurve * curve;
		size_t idx;
	};
}