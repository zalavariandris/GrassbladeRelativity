#include "CurveLocation.h"

CurveLocation::CurveLocation() :
	_curve(Curve()), _time(NAN) {};

CurveLocation::CurveLocation(Curve curve, double time) :
	_curve(curve), _time(time) {
	_point = _curve.getPointAtTime(_time);
	_normal = _curve.getNormal(_time, true);
	_tangent = _curve.getTangent(_time, true);
};

