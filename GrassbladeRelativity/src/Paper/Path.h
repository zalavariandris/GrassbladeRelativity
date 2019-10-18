#pragma once
#include "glm/glm.hpp"
#include "Segment.h"
#include "Curve.h"
#include "CurveLocation.h"

#include <vector>
#include <memory> // enable shared from this
#include <iostream>
namespace Paper {
	class Path
	{
		friend class Segment;
		
		// caching
		mutable bool CurvesNeedsUpdate{ true };
		mutable bool LengthNeedsUpdate{ true };
		mutable double mLength;

		// attributes
		bool mClosed{ false };

		// segments and curves
		mutable std::vector<Curve> mCurves;
		mutable std::vector<Segment> mSegments;

		// helper methods
		int countCurves() const;
		void _add(std::vector<Segment> segs, int index = -1);
		//Adjusts segments of curves before and after inserted / removed segments.
		void _adjustCurves(int start, int end);

	public:
		//constructors
		Path();
		Path(std::vector<Segment> segments);
		Path(const Path & other);
		Path& operator = (const Path &other);

		// accessors
		std::vector<Curve> & getCurves();
		Curve & getFirstCurve();
		Curve & getLastCurve();
		const std::vector<Curve> & getCurves() const;
		const Curve & getFirstCurve() const;
		const Curve & getLastCurve() const;

		std::vector<Segment> & getSegments();
		Segment & getFirstSegment();
		Segment & getLastSegment();
		const std::vector<Segment> & getSegments() const;
		const Segment & getFirstSegment() const;
		const Segment & getLastSegment() const;

		double getLength() const;
		bool closed() const;
		void closed(bool val);

		// get curve location
		CurveLocation getLocationAtTime(double t) const;
		glm::vec2 getPointAtTime(double t) const;
		glm::vec2 getTangentAtTime(double t) const;
		glm::vec2 getNormalAtTime(double t)const;

		CurveLocation getLocationAt(double offset) const;
		glm::vec2 getPointAt(double offset) const;
		glm::vec2 getTangentAt(double offset) const;
		glm::vec2 getNormalAt(double offset) const;

		double getNearestTime(glm::vec2 point) const;

		// mutate Path
		void add(Segment segment);
		void add(std::vector<Segment> segments);

		void insert(int index, Segment segment);
		void insert(int index, std::vector<Segment> segments);
	};
}

