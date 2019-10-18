#include "CurveLocation.h"
#include <array>
namespace Paper {
	CurveLocation::CurveLocation() {}

	CurveLocation::CurveLocation(Curve const & curve, double time) {
		mValid = curve.evaluate2(curve.getValues(), time, &mPoint, true, &mTangent, &mNormal);
		mTime = time;
	};
}

