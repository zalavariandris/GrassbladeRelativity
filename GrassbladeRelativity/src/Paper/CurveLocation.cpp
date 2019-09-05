#include "CurveLocation.h"
namespace Paper {
	CurveLocation::CurveLocation() :
		_curve(Curve()), _time(NAN) {};

	CurveLocation::CurveLocation(Curve curve, double time) :
		_curve(curve), _time(time) {
		_point = _curve.getPointAtTime(_time);
		_normal = _curve.getNormalAtTime(_time);
		_tangent = _curve.getTangentAtTime(_time);
	};
}

