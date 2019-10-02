#include "CurveLocation.h"
#include <array>
namespace Paper {
	CurveLocation::CurveLocation() :
		_curve(Curve()), _time(NAN) {};

	CurveLocation::CurveLocation(Curve curve, double time) :
		_curve(curve), _time(time) {
		_curve.evaluate2(curve.getValues(), _time, &_point, true, &_tangent, &_normal);
	};
}

