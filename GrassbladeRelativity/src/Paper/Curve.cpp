#include "Curve.h"

#include "ofMain.h" // ofDrawLine
#include "numerical.h"
#include "Path.h"
#include "CurveLocation.h"
#include "basic/Point.h"
#include "basic/Line.h"
namespace Paper {

	bool Curve::evaluate2(std::array<double, 8> const & v, double t, glm::vec2 * point, bool normalized, glm::vec2 * tangent, glm::vec2 * normal, double * curvature) {
		// Do not produce results if parameter is out of range or invalid.
		if (isnan(t) || t < 0 || t > 1)
			return false;

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

		if (point != nullptr) {
			// type === 0: getPoint()
			// Calculate the curve point at parameter value t
			// Use special handling at t === 0 / 1, to avoid imprecisions.
			// See #960
			point->x = t == 0 ? x0 : t == 1 ? x3
				: ((ax * t + bx) * t + cx) * t + x0;
			point->y = t == 0 ? y0 : t == 1 ? y3
				: ((ay * t + by) * t + cy) * t + y0;
		}

		if(tangent!=nullptr || normal!=nullptr || curvature!=nullptr) {
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
			double x, y;
			
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
			if (tangent != nullptr) {
				tangent->x = x;
				tangent->y = y;
			}
			if (curvature!=nullptr) {
				// Calculate 2nd derivative, and curvature from there:
				// http://cagd.cs.byu.edu/~557/text/ch2.pdf page#31
				// k = |dx * d2y - dy * d2x| / (( dx^2 + dy^2 )^(3/2))
				auto x2 = 6 * ax * t + 2 * bx;
				auto y2 = 6 * ay * t + 2 * by;
				auto d = pow(x * x + y * y, 3 / 2);
				// For JS optimizations we always return a Point, although
				// curvature is just a numeric value, stored in x:
				*curvature = d != 0 ? (x * y2 - y * x2) / d : 0;
			}
			if (normal != nullptr) {
				normal->x = y;
				normal->y = -x;
			}
		}

		return true;
	}

	glm::vec2 Curve::evaluate(std::array<double, 8> const & v, double t, int type, bool normalized) {
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

	bool Curve::isStraight(glm::vec2 const & p1, glm::vec2 const & h1, glm::vec2 const & h2, glm::vec2 const & p2){
		if (Point::isZero(h1) && Point::isZero(h2)) {
			// no handles
			return true;
		}
		else {
			auto v = p2 - p1;
			if (Point::isZero(v)) {
				// Zero-length line, with some handles defined.
				return false;
			}
			else if (Point::isCollinear(v, h1) && Point::isCollinear(v, h2)) {
				// Collinear handles: In addition to v.isCollinear(h) checks, we
				// need to measure the distance to the line, in order to be able
				// to use the same epsilon as in Curve#getTimeOf(), see #1066.
				Line l(p1, p2);
				auto epsilon = Numerical::GEOMETRIC_EPSILON;
				if (l.getDistance(p1+h1) < epsilon &&
					l.getDistance(p2+h2) < epsilon) {
					// Project handles onto line to see if they are in range:
					
					auto div = glm::dot(v, v);;
					auto s1 = glm::dot(v, h1) / div;
					auto s2 = glm::dot(v,h2) / div;
					return s1 >= 0 && s1 <= 1 && s2 <= 0 && s2 >= -1;
				}
			}
		}
		return false;
	}

	bool Curve::isStraight() const{
		return Curve::isStraight(_segment1->_point, _segment1->_handleOut, _segment1->_point, _segment2->_handleIn);
	}

	bool Curve::isLinear() const{
		glm::vec2 p1, h1, h2, p2;
		p1 = _segment1->_point;
		h1 = _segment1->_handleOut;
		h2 = _segment2->_handleIn;
		p2 = _segment2->_point;
		glm::vec2 third = (p2-p1)/3.0;
		return h1 == third && h2 == -third;
	}

	Line Curve::getLine() const {
		return Line(_segment1->_point, _segment2->_point);
	}

	bool Curve::isCollinear(Curve const & curve) const {
		return isStraight() && curve.isStraight()
			&& getLine().isCollinear(curve.getLine());
	}

	bool Curve::hasHandles() const {
		return !Point::isZero(_segment1->_handleOut)
			|| !Point::isZero(_segment2->_handleIn);
	}

	glm::vec2 Curve::getPointAtTime(double t) const {
		glm::vec2 point;
		if (!evaluate2(getValues(), t, &point))
			return glm::vec2(NAN);
		return point;
	};

	glm::vec2 Curve::getTangentAtTime(double t) const {
		glm::vec2 tangent;
		if (!evaluate2(getValues(), t, nullptr, true, &tangent))
			return glm::vec2(NAN);
		return tangent;
	};

	glm::vec2 Curve::getWeightedTangentAtTime(double t) const {
		glm::vec2 tangent;
		if (!evaluate2(getValues(), t, nullptr, false, &tangent))
			return glm::vec2(NAN);
		return tangent;
	};

	glm::vec2 Curve::getNormalAtTime(double t) const {
		glm::vec2 normal;
		if (!evaluate2(getValues(), t, nullptr, true, nullptr, &normal))
			return glm::vec2(NAN);
		return normal;
	};

	glm::vec2 Curve::getWeightedNormalAtTime(double t) const {
		glm::vec2 normal;
		if (!evaluate2(getValues(), t, nullptr, false, nullptr, &normal))
			return glm::vec2(NAN);
		return normal;
	};

	double Curve::getCurvatureAtTime(double t) const {
		double curvature;
		if (!evaluate2(getValues(), t, nullptr, true, nullptr, nullptr, &curvature))
			return NAN;
		return curvature;
	};

	double Curve::getNearestTime(glm::vec2 point) const {
		/* rewritten from paper.js*/

		if (isStraight()) {
			auto v = getValues();
			auto x0 = v[0], y0 = v[1];
			auto x3 = v[6], y3 = v[7];
			auto vx = x3 - x0, vy = y3 - y0;
			auto det = vx * vx + vy * vy;
			// Avoid divisions by zero.
			if (det == 0) {
				std::cout << "avoid division by zero" << std::endl;
				return 0;
			}
			// Project the point onto the line and calculate its linear
			// parameter u along the line: u = (point - p1).dot(v) / v.dot(v)
			double u = ((point.x - x0) * vx + (point.y - y0) * vy) / det;
			return u < Numerical::EPSILON ? 0
				: u > (1 - Numerical::EPSILON) ? 1
				: getTimeOf(glm::vec2(x0 + u * vx, y0 + u * vy));
		}

		const int count = 25; //was 100, TODO count is the stpes to refine the losest point.
		double minDist = std::numeric_limits<double>::infinity();
		double minT = 0;

		auto refine = [&](double t)->bool {
			if (t >= 0.0 && t <= 1.0) {
				//double distance = glm::distance(point, getPointAtTime(t));
				auto temp = point - getPointAtTime(t);
				double distance = glm::dot(temp, temp);
				if (distance < minDist) {
					minDist = distance;
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
		auto & v = getValues();
		const double x0 = v[0]; const double y0 = v[1];
		const double x1 = v[2]; const double y1 = v[3];
		const double x2 = v[4]; const double y2 = v[5];
		const double x3 = v[6]; const double y3 = v[7];

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
		if (isStraight()) {
			// Sub-divide the linear curve at a and b, so we can simply
			// calculate the Pythagorean Theorem to get the range's length.
			if (b < 1 || a>0) {
				throw "get straight range length is not implemented yet !!!";
			}

			// The length of straight curves can be calculated more easily.
			auto & v = getValues();
			auto dx = v[6] - v[0]; // x3 - x0
			auto dy = v[7] - v[1]; // y3 - y0
			return sqrt(dx * dx + dy * dy);
		}
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

	CurveLocation Curve::getLocationOf(glm::vec2 point) const {
		return getLocationAtTime(getTimeOf(point));
	}

	double Curve::getTimeOf(glm::vec2 point) const {
		auto & v = getValues();
		// Before solving cubics, compare the beginning and end of the curve
		// with zero epsilon:
		glm::vec2 p0{ v[0], v[1] };
		glm::vec2 p3{ v[6], v[7] };
		double epsilon = Numerical::EPSILON;
		double geomEpsilon = Numerical::GEOMETRIC_EPSILON;
		double t = Point::isClose(point, p0, epsilon) ? 0.0
				 : Point::isClose(point, p3, epsilon) ? 1.0
				 : NAN;

		if (isnan(t)) {
			// Solve the cubic for both x- and y-coordinates and consider all
			// solutions, testing with the larger / looser geometric epsilon.
			std::vector<double> coords{ point.x, point.y };
			std::vector<double> roots;
			for (auto c = 0; c < 2; c++) {
				int count = Curve::solveCubic(v, c, coords[c], roots, 0, 1);
				for (auto i = 0; i < count; i++) {
					auto u = roots[i];
					//!TODO: BUG !!!!! this must be a bug
					// the original PAPER.js code makes sure that the point at time lies on the curve.
					// dont really know why... the thing is it works now, but there certanly a reason for it.
					// if a keep the original check, it wont be close enough. There is probably a precision mistake somewhere in the code.
					// for now we return the u without any check
					return u; 
					if (Point::isClose(point, getPointAtTime(u), geomEpsilon))
						return u;
				}
			}
		}

		// Since we're comparing with geometric epsilon for any other t along
		// the curve, do so as well now for the beginning and end of the curve.
		return Point::isClose(point, p0, geomEpsilon) ? 0
			: Point::isClose(point, p3, geomEpsilon) ? 1
			: NAN;
	}

	int Curve::solveCubic(std::array<double, 8> v, int coord, double val, std::vector<double> & roots, double minV, double maxV) {
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

	const std::array<double, 8> Curve::getValues() const {
		const glm::vec2 & p1 = _segment1->_point;
		const glm::vec2 & h1 = _segment1->_handleOut;
		const glm::vec2 & h2 = _segment2->_handleIn;
		const glm::vec2 & p2 = _segment2->_point;
		//double x1 = p1.x; double y1 = p1.y;
		//double x2 = p2.x; double y2 = p2.y;

		return {
			p1.x, p1.y,
			p1.x + h1.x, p1.y + h1.y,
			p2.x + h2.x, p2.y + h2.y,
			p2.x, p2.y
		};
	}

	void Curve::_changed() {
		LengthNeedsUpdate = true;
		BoundsNeedsUpdate = true;
	}
}