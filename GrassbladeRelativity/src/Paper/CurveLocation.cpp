#include "CurveLocation.h"

CurveLocation::CurveLocation() :
	_curve(Curve()), _time(NAN) {};

CurveLocation::CurveLocation(Curve curve, double time) :
	_curve(curve), _time(time) {
	_point = _curve.getPointAtTime(time);
};

