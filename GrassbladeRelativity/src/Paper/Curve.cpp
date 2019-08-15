#include "ofMain.h" // ofDrawLine
#include "Curve.h"
#include "numerical.h"


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

	float tMin = Numerical::CURVETIME_EPSILON;
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

double Curve::getNearestTime(glm::vec2 point) {
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
	while (step > Numerical::CURVETIME_EPSILON) {
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

CurveLocation Curve::getLocationAtTime(double t) {
	return CurveLocation(weak_from_this(), t);
}

//TODO: originaly isClose is part of the point class. Check how PAPER.js handles epsilon parameter?
bool isClose(glm::vec2 P, glm::vec2 Q, double epsilon = Numerical::EPSILON) {
	return glm::distance(P, Q) < epsilon;
}

CurveLocation Curve::getLocationOf(glm::vec2 point) {
	return getLocationAtTime(getTimeOf(point));
}

double Curve::getTimeOf(glm::vec2 point) {
	glm::vec2 p0 = A;
	glm::vec2 p3 = B;
	double t = isClose(point, p0) ? 0
		: isClose(point, p3) ? 1
		: NAN;

	if (isnan(t)) {
		std::vector<double> coords{ point.x, point.y };
		std::vector<double> roots;
		for (auto c = 0; c < 2; c++) {
			//TODO: in paperjs there is a private v parameter of curve, holdin all the coordinates of the curve instead 4 points
			// for now supply an array of the point coords, but later implement the v aparameter as in paperjs, and use that.
			auto count = Curve::solveCubic({A.x, A.y, B.x, B.y, C.x, C.y, D.x, D.y}, c, coords[c], roots, 0, 1);
			for (auto i = 0; i < count; i++) {
				auto u = roots[i];
				if (isClose(point, getPoint(u), Numerical::GEOMETRIC_EPSILON))
					return u;
			}
		}
	}
}

double Curve::solveCubic(std::vector<double> v, int coord, double val, std::vector<double> & roots, double minV, double maxV) {
	auto v0 = v[coord];
	auto v1 = v[coord + 2];
	auto v2 = v[coord + 4];
	auto v3 = v[coord + 6];
	auto res = 0;

	// If val is outside the curve values, no solution is possible.
	if (!(v0 < val && v3 < val && v1 < val && v2 < val ||
		v0 > val && v3 > val && v1 > val && v2 > val)) {
		auto c = 3 * (v1 - v0);
		auto b = 3 * (v2 - v1) - c;
		auto a = v3 - v0 - c - b;
		res = Numerical::solveCubic(a, b, c, v0 - val, roots, minV, maxV);
	}
	return res;
}