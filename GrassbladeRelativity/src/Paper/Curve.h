#pragma once

#include "glm/glm.hpp"
#include <functional>
#include <limits>
#include <memory> //shared_ptr, unique_ptr, weak_ptr

class CurveLocation; // forward declare 

class Curve : public std::enable_shared_from_this<Curve> {
private:
	glm::vec2 A; // The first anchor point of the curve.
	glm::vec2 D; // The second anchor point of the curve.
	glm::vec2 B; // The handle point that describes the tangent in the first anchor point.
	glm::vec2 C; // The handle point that describes the tangent in the second anchor point.

	std::function<double(double)> getLengthIntegrand();

	// Amount of integral evaluations for the interval 0 <= a < b <= 1
	int getIterations(double a, double b);

public:
	Curve() :
		A(glm::vec2()), B(glm::vec2()), C(glm::vec2()), D(glm::vec2()) {};

	Curve(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 D) :
		A(A), B(B), C(C), D(D) {};

	glm::vec2 getPoint(float t);

	glm::vec2 getTangent(float t, bool normalized = true);

	glm::vec2 getNormal(float t, bool normalized = true);

	double getNearestTime(glm::vec2 point);

	double getLength();

	void draw();

	CurveLocation getLocationOf(glm::vec2 point) {
		return getLocationAtTime(getTimeOf(point));
	}

	double getTimeOf(glm::vec2 point);

	CurveLocation getLocationAtTime(double t);
};

class CurveLocation {
private:
	double time;
	std::weak_ptr<Curve> curve;
public:
	CurveLocation(std::weak_ptr<Curve> curve, double time) :
		curve(curve), time(time) {};
};
