#include "Curve.h"
#include "numerical.h"
#include "ofMain.h" // ofDrawLine


glm::vec2 Curve::getPoint(float t) {
	// from paper.js
	// type === 0: getPoint()
	// Calculate the curve point at parameter value t
	// Use special handling at t === 0 / 1, to avoid imprecisions.
	// See #960
	float x0 = A.x;
	float y0 = A.y;
	float x1 = B.x;
	float y1 = B.y;
	float x2 = C.x;
	float y2 = C.y;
	float x3 = D.x;
	float y3 = D.y;

	// Calculate the polynomial coefficients.
	float cx = 3 * (x1 - x0);
	float bx = 3 * (x2 - x1) - cx;
	float ax = x3 - x0 - cx - bx;
	float cy = 3 * (y1 - y0);
	float by = 3 * (y2 - y1) - cy;
	float ay = y3 - y0 - cy - by;
	float x, y;

	x = t == 0 ? x0 : t == 1 ? x3
		: ((ax * t + bx) * t + cx) * t + x0;
	y = t == 0 ? y0 : t == 1 ? y3
		: ((ay * t + by) * t + cy) * t + y0;

	return glm::vec2(x, y);

	// my geometryic based cubic bezier, old
	//glm::vec2 P1 = glm::mix(A, B, t);
	//glm::vec2 Q1 = glm::mix(B, C, t);
	//glm::vec2 R1 = glm::mix(C, D, t);

	//glm::vec2 P2 = glm::mix(P1, Q1, t);
	//glm::vec2 Q2 = glm::mix(Q1, R1, t);

	//glm::vec2 P3 = glm::mix(P2, Q2, t);
	//return P3;
}

glm::vec2 Curve::getTangent(float t, bool normalized) {
	float x0 = A.x;
	float y0 = A.y;
	float x1 = B.x;
	float y1 = B.y;
	float x2 = C.x;
	float y2 = C.y;
	float x3 = D.x;
	float y3 = D.y;

	float cx = 3 * (x1 - x0);
	float bx = 3 * (x2 - x1) - cx;
	float ax = x3 - x0 - cx - bx;
	float cy = 3 * (y1 - y0);
	float by = 3 * (y2 - y1) - cy;
	float ay = y3 - y0 - cy - by;
	float x, y;

	float tMin = CURVETIME_EPSILON;
	float tMax = 1 - tMin;
	if (t < tMin) {
		x = cx;
		y = cy;
	}
	else if (t > tMax) {
		x = 3 * (x3 - x2);
		y = 3 * (y3 - y2);
	}
	else {
		x = (3 * ax * t + 2 * bx) * t + cx;
		y = (3 * ay * t + 2 * by) * t + cy;
	}

	return normalized ? glm::normalize(glm::vec2(x, y)) : glm::vec2(x, y);
}

glm::vec2 Curve::getNormal(float t, bool normalized) {
	glm::vec2 tangent = getTangent(t, normalized);
	return glm::vec2(tangent.y, -tangent.x);
}

float Curve::getNearestTime(glm::vec2 point) {
	/* rewritten from paper.js*/
	const int count = 100;
	float minDist = std::numeric_limits<float>::infinity();
	float minT = 0;

	static auto refine = [&](float t)->bool {
		if (t >= 0.0 && t <= 1.0) {
			float dist = glm::distance(point, getPoint(t));
			if (dist < minDist) {
				minDist = dist;
				minT = t;
				return true;
			}
			return false;
		}
		return false;
	};

	for (int i = 0; i <= count; i++) {
		refine((float)i / count);
	};

	float step = 1.0f / (count * 2);
	while (step > CURVETIME_EPSILON) {
		if (!refine(minT - step) && !refine(minT + step)) {
			step /= 2.0;
		}
	}
	return minT;
}

std::function<double(double)> Curve::getLengthIntegrand() {
	// Calculate the coefficients of a Bezier derivative.
	double x0 = A.x; double y0 = A.y;
	double x1 = B.x; double y1 = B.y;
	double x2 = C.x; double y2 = C.y;
	double x3 = D.x; double y3 = D.y;

	double ax = 9 * (x1 - x2) + 3 * (x3 - x0);
	double bx = 6 * (x0 + x2) - 12 * x1;
	double cx = 3 * (x1 - x0);

	double ay = 9 * (y1 - y2) + 3 * (y3 - y0);
	double by = 6 * (y0 + y2) - 12 * y1;
	double cy = 3 * (y1 - y0);

	return [=](double t)->double {
		// Calculate quadratic equations of derivatives for x and y
		double dx = (ax * t + bx) * t + cx;
		double dy = (ay * t + by) * t + cy;
		return sqrt(dx * dx + dy * dy);
	};
}

// Amount of integral evaluations for the interval 0 <= a < b <= 1
int Curve::getIterations(double a, double b) {
	// Guess required precision based and size of range...
	// TODO: There should be much better educated guesses for
	// this. Also, what does this depend on? Required precision?

	return Numerical::getMax({ 2, Numerical::getMin({ 16, ceil(abs(b - a) * 32) }) });
}

double Curve::getLength() {
	double a = 0.0;
	double b = 1.0;
	return Numerical::integrate(getLengthIntegrand(), a, b, getIterations(a, b));
}

void Curve::draw() {
	const int segments{ 8 };
	for (int i = 0; i < segments; i++) {
		glm::vec2 P1 = getPoint((float)i / segments);
		glm::vec2 P2 = getPoint(((float)i + 1) / segments);
		ofDrawLine(P1, P2);
	};
}


