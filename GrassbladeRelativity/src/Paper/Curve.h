#pragma once

#include "glm/glm.hpp"
#include <functional>
#include <limits>
#include <memory> //shared_ptr, unique_ptr, weak_ptr
#include <vector>
#include "Segment.h"
#include "basic/Line.h"
namespace Paper {
	class Path;
	class CurveLocation;
	class Curve : public std::enable_shared_from_this<Curve> {
	private:
		std::function<double(double)> getLengthIntegrand() const;

		// Amount of integral evaluations for the interval 0 <= a < b <= 1
		int getIterations(double a, double b) const;

		bool LengthNeedsUpdate{ true };
		bool BoundsNeedsUpdate{ true };
	public:
		/*
		 *static functions
		 */
		static glm::vec2 evaluate(std::array<double, 8> const & v, double t, int type, bool normalized);
		static bool evaluate2(std::array<double, 8> const & v, double t, glm::vec2 * point = nullptr, bool normalized = true, glm::vec2 * tangent = nullptr, glm::vec2 * normal = nullptr, double * curvature = nullptr);
		static int solveCubic(std::array<double, 8> v, int coord, double val, std::vector<double> & roots, double minV, double maxV);
		static bool isStraight(glm::vec2 const & p1, glm::vec2 const & h1, glm::vec2 const & h2, glm::vec2 const & p2);

		/*
		 * private memnbers
		 */
		std::shared_ptr<Segment> _segment1;
		std::shared_ptr<Segment> _segment2;

		// class constructors
		Curve() :_segment1(std::make_shared<Segment>()), _segment2(std::make_shared<Segment>()) {};
		Curve(std::shared_ptr<Segment> segment1, std::shared_ptr<Segment> segment2) : _segment1(segment1), _segment2(segment2) {};

		/* Undocumented internal constructor, used by Path#getCurves()
		 * new Segment(path, segment1, segment2); */
		 //TODO: weak reference to path
		 //Curve(std::weak_ptr<Path> path, std::shared_ptr<Segment> segment1, std::shared_ptr<Segment> segment2): _path(path), _segment1(segment1), _segment2(segment2) {};

		 /*
		  * member functions
		  */

		bool isStraight() const;

		bool hasHandles() const;

		bool isLinear() const;

		bool isCollinear(Curve const & curve) const;

		Line getLine() const;

		std::array<double, 8> getValues() const; //TODO: !!! is this public?

		glm::vec2 getPointAtTime(double t) const;

		glm::vec2 getTangentAtTime(double t) const;

		glm::vec2 getWeightedTangentAtTime(double t) const;

		glm::vec2 getNormalAtTime(double t) const;

		glm::vec2 getWeightedNormalAtTime(double t) const;

		double getCurvatureAtTime(double t) const;

		//
		double getNearestTime(glm::vec2 point) const;

		double getLength(double a = 0.0, double b = 1.0) const;

		double getLength(double a, double b, std::function<double(double)> ds) const;

		void draw() const;

		CurveLocation getLocationAt(double offset) const;

		CurveLocation getLocationAtTime(double t) const;

		double getTimeAt(double offset, double start = NAN) const;

		/**
		* {@grouptitle Positions on Paths and Curves}
		*
		* Returns the curve location of the specified point if it lies on the
		* path, `null` otherwise.
		*
		* @param {Point} point the point on the path
		* @return {CurveLocation} the curve location of the specified point
		*/
		CurveLocation getLocationOf(glm::vec2 point) const;

		double getTimeOf(glm::vec2 point) const;

		void _changed();
	};
}