#pragma once

#include "glm/glm.hpp"
#include <functional>
#include <limits>
#include <memory> //shared_ptr, unique_ptr, weak_ptr
#include <vector>
#include "Segment.h"

class Path;
class CurveLocation;
class Curve : public std::enable_shared_from_this<Curve> {
private:
	std::function<double(double)> getLengthIntegrand();

	// Amount of integral evaluations for the interval 0 <= a < b <= 1
	int getIterations(double a, double b);
	std::weak_ptr<Path> _path;
	std::shared_ptr<Segment> _segment1;
	std::shared_ptr<Segment> _segment2;

public:
	Curve():
	_segment1(std::make_shared<Segment>()),
	_segment2(std::make_shared<Segment>()){};

	Curve(std::shared_ptr<Segment> segment1, std::shared_ptr<Segment> segment2):
		_segment1(segment1), 
		_segment2(segment2) {};

	/* Undocumented internal constructor, used by Path#getCurves()
	 * new Segment(path, segment1, segment2);
	 */
	Curve(std::weak_ptr<Path> path, std::shared_ptr<Segment> segment1, std::shared_ptr<Segment> segment2):
		_path(path), 
		_segment1(segment1), 
		_segment2(segment2) {};

	std::vector<double> getValues(); //TODO: !!! is this public?

	glm::vec2 getPointAtTime(double t);

	glm::vec2 getTangent(double t, bool normalized = true);

	glm::vec2 getNormal(double t, bool normalized = true);

	double getCurvature(double t);

	double getNearestTime(glm::vec2 point);

	double getLength(double a=0.0, double b=1.0);

	double getLength(double a, double b, std::function<double(double)> ds);

	void draw();

	CurveLocation getLocationAt(double offset);

	CurveLocation getLocationAtTime(double t);

	double getTimeAt(double offset, double start = NAN);

	/**
	* {@grouptitle Positions on Paths and Curves}
	*
	* Returns the curve location of the specified point if it lies on the
	* path, `null` otherwise.
	*
	* @param {Point} point the point on the path
	* @return {CurveLocation} the curve location of the specified point
	*/
	CurveLocation getLocationOf(glm::vec2 point);

	double getTimeOf(glm::vec2 point);

	static double solveCubic(std::vector<double> v, int coord, double val, std::vector<double> & roots, double minV, double maxV);
};
