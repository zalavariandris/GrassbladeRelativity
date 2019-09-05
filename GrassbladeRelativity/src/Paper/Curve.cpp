#include "Curve.h"

#include "ofMain.h" // ofDrawLine
#include "numerical.h"
#include "Path.h"
#include "CurveLocation.h"
namespace Paper {
	namespace Point {
		bool isZero(glm::vec2 P) {
			return Numerical::isZero(P.x) && Numerical::isZero(P.y);
		}

		bool isCollinear(glm::vec2 A, glm::vec2 B) {
			// NOTE: We use normalized vectors so that the epsilon comparison is
			// reliable. We could instead scale the epsilon based on the vector
			// length. But instead of normalizing the vectors before calculating
			// the cross product, we can scale the epsilon accordingly.
			return abs(A.x * B.y - A.y * B.x)
				<= sqrt((A.x * A.x + A.y * A.y) * (B.x * B.x + B.y * B.y))
				* /*#=*/Numerical::TRIGONOMETRIC_EPSILON;
		}
	}

	glm::vec2 Curve::evaluate(std::array<double, 8> v, double t, int type, bool normalized) {
		// Do not produce results if parameter is out of range or invalid.
		if (isnan(t) || t < 0 || t > 1)
			return glm::vec2(NAN);

		double x0 = v[0]; double y0 = v[1];
		double x1 = v[2]; double y1 = v[3];
		double x2 = v[4]; double y2 = v[5];
		double x3 = v[6]; double y3 = v[7];

		// If the curve handles are almost zero, reset the control points to the
		// anchors.
		if (Numerical::isZero(x1 - x0) && Numerical::isZero(y1 - y0)) {
			x1 = x0;
			y1 = y0;
		}
		if (Numerical::isZero(x2 - x3) && Numerical::isZero(y2 - y3)) {
			x2 = x3;
			y2 = y3;
		}

		// Calculate the polynomial coefficients.
		double cx = 3 * (x1 - x0);
		double bx = 3 * (x2 - x1) - cx;
		double ax = x3 - x0 - cx - bx;
		double cy = 3 * (y1 - y0);
		double by = 3 * (y2 - y1) - cy;
		double ay = y3 - y0 - cy - by;
		double x, y;

		if (type == 0) {
			// type === 0: getPoint()
			// Calculate the curve point at parameter value t
			// Use special handling at t === 0 / 1, to avoid imprecisions.
			// See #960
			x = t == 0 ? x0 : t == 1 ? x3
				: ((ax * t + bx) * t + cx) * t + x0;
			y = t == 0 ? y0 : t == 1 ? y3
				: ((ay * t + by) * t + cy) * t + y0;
		}
		else {
			// type === 1: getTangent()
			// type === 2: getNormal()
			// type === 3: getCurvature()
			double tMin = Numerical::CURVETIME_EPSILON;
			double tMax = 1 - tMin;
			// 1: tangent, 1st derivative
			// 2: normal, 1st derivative
			// 3: curvature, 1st derivative & 2nd derivative
			// Prevent tangents and normals of length 0:
			// https://stackoverflow.com/questions/10506868/
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
			if (normalized) {
				// When the tangent at t is zero and we're at the beginning
				// or the end, we can use the vector between the handles,
				// but only when normalizing as its weighted length is 0.
				if (x == 0 && y == 0 && (t < tMin || t > tMax)) {
					x = x2 - x1;
					y = y2 - y1;
				}
				// Now normalize x & y
				double len = sqrt(x * x + y * y);
				if (len) {
					x /= len;
					y /= len;
				}
			}
			if (type == 3) {
				// Calculate 2nd derivative, and curvature from there:
				// http://cagd.cs.byu.edu/~557/text/ch2.pdf page#31
				// k = |dx * d2y - dy * d2x| / (( dx^2 + dy^2 )^(3/2))
				auto x2 = 6 * ax * t + 2 * bx;
				auto y2 = 6 * ay * t + 2 * by;
				auto d = pow(x * x + y * y, 3 / 2);
				// For JS optimizations we always return a Point, although
				// curvature is just a numeric value, stored in x:
				x = d != 0 ? (x * y2 - y * x2) / d : 0;
				y = 0;
			}
		}
		// The normal is simply the rotated tangent:
		return type == 2 ? glm::vec2(y, -x) : glm::vec2(x, y);
	}

	glm::vec2 Curve::getPointAtTime(double t) const {
		return evaluate(getValues(), t, 0, false);
	};

	glm::vec2 Curve::getTangentAtTime(double t) const {
		return evaluate(getValues(), t, 1, true);
	};

	glm::vec2 Curve::getWeightedTangentAtTime(double t) const {
		return evaluate(getValues(), t, 1, false);
	};

	glm::vec2 Curve::getNormalAtTime(double t) const {
		return evaluate(getValues(), t, 2, true);
	};

	glm::vec2 Curve::getWeightedNormalAtTime(double t) const {
		return evaluate(getValues(), t, 2, false);
	};

	double Curve::getCurvatureAtTime(double t) const {
		return evaluate(getValues(), t, 3, false).x;
	};

	double Curve::getNearestTime(glm::vec2 point) const {
		/* rewritten from paper.js*/
		const int count = 100;
		double minDist = std::numeric_limits<double>::infinity();
		double minT = 0;

		auto refine = [&](double t)->bool {
			if (t >= 0.0 && t <= 1.0) {
				double dist = glm::distance(point, getPointAtTime(t));
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
			refine((double)i / count);
		};

		double step = 1.0f / (count * 2);
		while (step > Numerical::CURVETIME_EPSILON) {
			if (!refine(minT - step) && !refine(minT + step)) {
				step /= 2.0;
			}
		}
		return minT;
	}

	std::function<double(double)> Curve::getLengthIntegrand() const {
		auto v = getValues();
		double x0 = v[0]; double y0 = v[1];
		double x1 = v[2]; double y1 = v[3];
		double x2 = v[4]; double y2 = v[5];
		double x3 = v[6]; double y3 = v[7];

		// Calculate the coefficients of a Bezier derivative.
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
	int Curve::getIterations(double a, double b) const {
		// Guess required precision based and size of range...
		// TODO: There should be much better educated guesses for
		// this. Also, what does this depend on? Required precision?

		return Numerical::getMax({ 2, Numerical::getMin({ 16, ceil(abs(b - a) * 32) }) });
	}

	double Curve::getLength(double a, double b) const {
		return getLength(a, b, getLengthIntegrand());
	}

	double Curve::getLength(double a, double b, std::function<double(double)> ds) const {
		return Numerical::integrate(ds, a, b, getIterations(a, b));
	}

	void Curve::draw() const {
		const int segments{ 8 };
		for (int i = 0; i < segments; i++) {
			glm::vec2 P1 = getPointAtTime((double)i / segments);
			glm::vec2 P2 = getPointAtTime(((double)i + 1) / segments);
			ofDrawLine(P1, P2);
		};
	}

	CurveLocation Curve::getLocationAtTime(double t) const {
		return CurveLocation(*this, t);
	}

	//TODO: originaly isClose is part of the point class. Check how PAPER.js handles epsilon parameter?
	bool isClose(glm::vec2 P, glm::vec2 Q, double epsilon = Numerical::EPSILON) {
		return glm::distance(P, Q) < epsilon;
	}

	CurveLocation Curve::getLocationOf(glm::vec2 point) const {
		return getLocationAtTime(getTimeOf(point));
	}

	double Curve::getTimeOf(glm::vec2 point) const {
		auto v = getValues();
		glm::vec2 p0{ v[0], v[1] };
		glm::vec2 p3{ v[6], v[7] };
		double t = isClose(point, p0) ? 0
			: isClose(point, p3) ? 1
			: NAN;

		if (isnan(t)) {
			std::vector<double> coords{ point.x, point.y };
			std::vector<double> roots;
			for (auto c = 0; c < 2; c++) {
				//TODO: in paperjs there is a private v parameter of curve, holdin all the coordinates of the curve instead 4 points
				// for now supply an array of the point coords, but later implement the v aparameter as in paperjs, and use that.
				auto count = Curve::solveCubic(v, c, coords[c], roots, 0, 1);
				for (auto i = 0; i < count; i++) {
					auto u = roots[i];
					if (isClose(point, getPointAtTime(u), Numerical::GEOMETRIC_EPSILON))
						return u;
				}
			}
		}
	}

	double Curve::solveCubic(std::array<double, 8> v, int coord, double val, std::vector<double> & roots, double minV, double maxV) {
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

	CurveLocation Curve::getLocationAt(double offset) const {
		return getLocationAtTime(getTimeAt(offset));
	}

	double Curve::getTimeAt(double offset, double start) const {
		if (isnan(start))
			start = offset < 0 ? 1 : 0;
		if (offset == 0)
			return start;

		// See if we're going forward or backward, and handle cases
		// differently
		auto epsilon = /*#=*/Numerical::EPSILON;
		auto forward = offset > 0;
		auto a = forward ? start : 0;
		auto b = forward ? 1 : start;
		// Use integrand to calculate both range length and part
		// lengths in f(t) below.
		auto ds = getLengthIntegrand();
		// Get length of total range
		auto rangeLength = getLength(a, b, ds);
		auto diff = abs(offset) - rangeLength;
		if (abs(diff) < epsilon) {
			// Matched the end:
			return forward ? b : a;
		}
		else if (diff > epsilon) {
			// We're out of bounds.
			return NAN;
		}
		// Use offset / rangeLength for an initial guess for t, to
		// bring us closer:
		double guess = offset / rangeLength;
		double length = 0.0;
		// Iteratively calculate curve range lengths, and add them up,
		// using integration precision depending on the size of the
		// range. This is much faster and also more precise than not
		// modifying start and calculating total length each time.
		auto f = [&](double t) {
			// When start > t, the integration returns a negative value.
			length += Numerical::integrate(ds, start, t,
				getIterations(start, t));
			start = t;
			return length - offset;
		};
		// Start with out initial guess for x.
		// NOTE: guess is a negative value when looking backwards.
		return Numerical::findRoot(f, ds, start + guess, a, b, 32,
			/*#=*/Numerical::EPSILON);
	}

	std::array<double, 8> Curve::getValues() const {
		glm::vec2 p1 = _segment1->_point;
		glm::vec2 h1 = _segment1->_handleOut;
		glm::vec2 h2 = _segment2->_handleIn;
		glm::vec2 p2 = _segment2->_point;
		double x1 = p1.x; double y1 = p1.y;
		double x2 = p2.x; double y2 = p2.y;

		return {
			x1, y1,
			x1 + h1.x, y1 + h1.y,
			x2 + h2.x, y2 + h2.y,
			x2, y2
		};
	}

	void Curve::_changed() {
		LengthNeedsUpdate = true;
		BoundsNeedsUpdate = true;
	}
}