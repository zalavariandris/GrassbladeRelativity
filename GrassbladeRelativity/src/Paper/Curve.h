#pragma once

#include "glm/glm.hpp"
#include <functional>
#include <limits>

class Curve {
private:
	glm::vec2 A; // The first anchor point of the curve.
	glm::vec2 D; // The second anchor point of the curve.
	glm::vec2 B; // The handle point that describes the tangent in the first anchor point.
	glm::vec2 C; // The handle point that describes the tangent in the second anchor point.
	const float CURVETIME_EPSILON{ 0.000000001 };//10 ^ -8 // 1e-8,

	std::function<double(double)> getLengthIntegrand();

	// Amount of integral evaluations for the interval 0 <= a < b <= 1
	int getIterations(double a, double b);

public:
	Curve(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 D) :
		A(A), B(B), C(C), D(D) {};

	glm::vec2 getPoint(float t);

	glm::vec2 getTangent(float t, bool normalized = true);

	glm::vec2 getNormal(float t, bool normalized = true);

	float getNearestTime(glm::vec2 point);

	double getLength();

	void draw();
};
