#include "CurveLocation.h"
#include "Curve.h"

CurveLocation::CurveLocation() :
	_curve(std::shared_ptr<Curve>(nullptr)), _time(NAN) {};

CurveLocation::CurveLocation(std::shared_ptr<Curve> curve, double time) :
	_curve(curve), _time(time) {
	if(_curve)
		_point = _curve->getPointAtTime(time);
};

