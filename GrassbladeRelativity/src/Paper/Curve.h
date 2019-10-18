#pragma once

#include "glm/glm.hpp"
#include <functional>
#include <limits>
#include <memory> //shared_ptr, unique_ptr, weak_ptr
#include <vector>
#include "Segment.h"
#include "basic/Line.h"
#include <array>
namespace Paper {
	class Path;
	class CurveLocation;
	class Curve {
	private:
		friend class Path;
		bool LengthNeedsUpdate{ true };
		bool BoundsNeedsUpdate{ true };
		Segment * mSegment1;
		Segment * mSegment2;
		Path * mPath{ nullptr };

		Curve(Path * path, Segment * segment1, Segment * segment2);
	public:
		// Static functions
		//static glm::vec2 evaluate(std::array<double, 8> const & v, double t, int type, bool normalized);
		//
		static bool evaluate2(std::array<double, 8> const & v, double t, glm::vec2 * point = nullptr, bool normalized = true, glm::vec2 * tangent = nullptr, glm::vec2 * normal = nullptr, double * curvature = nullptr);
		static bool isStraight(glm::vec2 const & p1, glm::vec2 const & h1, glm::vec2 const & h2, glm::vec2 const & p2);

		// helper functions
		std::function<double(double)> getLengthIntegrand() const;
		// Amount of integral evaluations for the interval 0 <= a < b <= 1
		int getIterations(double a, double b) const;


		// constructors
		Curve();
		Curve(Segment * segment1, Segment * segment2);
		Curve(double x1, double y1, double handle1x, double handle1y, double handle2x, double handle2y, double x2, double y2);
		~Curve();

		// accessors
		Segment * segment1();
		const Segment * segment1() const;
		Segment * segment2();
		const Segment * segment2() const;

		const std::array<double, 8> getValues() const;
		const Line getLine() const;
		bool isStraight() const;
		bool hasHandles() const;
		bool isLinear() const;
		bool isCollinear(Curve const & curve) const;

		double getLength(double a = 0.0, double b = 1.0) const;
		double getLength(double a, double b, std::function<double(double)> ds) const;

		std::pair<Curve, Curve> Curve::subdivide(double t=NAN) const;
		
		/* get path locations */
		CurveLocation getLocationAtTime(double t) const;
		glm::vec2 getPointAtTime(double t) const;
		glm::vec2 getTangentAtTime(double t) const;
		glm::vec2 getWeightedTangentAtTime(double t) const;
		glm::vec2 getNormalAtTime(double t) const;
		glm::vec2 getWeightedNormalAtTime(double t) const;
		double getCurvatureAtTime(double t) const;

		CurveLocation getLocationAt(double offset) const;

		double getNearestTime(glm::vec2 point) const;
		double getTimeAt(double offset, double start = NAN) const;

		//Returns the curve location of the specified point if it lies on the path, `null` otherwise.
		CurveLocation getLocationOf(glm::vec2 point) const;
		double getTimeOf(glm::vec2 point) const;
	};
}