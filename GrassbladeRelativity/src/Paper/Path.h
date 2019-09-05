#pragma once
#include "glm/glm.hpp"
#include "Segment.h"
#include "Curve.h"
#include "CurveLocation.h"

#include <vector>
#include <memory> // enable shared from this

namespace Paper {
	class Path : public std::enable_shared_from_this<Path>
	{

		int _countCurves() const;
		bool _closed;
		int _version;

		// caching
		mutable bool CurvesNeedsUpdate{ true };
		mutable bool LengthNeedsUpdate{ true };
		mutable double _length;

		mutable std::vector<std::shared_ptr<Curve>> _curves;
		std::vector<std::shared_ptr<Segment>> _segments;
	public:


		//constructor
		Path();
		Path(std::vector<std::shared_ptr<Segment>> segments);

		// accessors
		std::vector<std::shared_ptr<Curve>> getCurves() const;

		//
		CurveLocation getLocationAt(double offset) const;
		glm::vec2 getPointAt(double offset) const;
		glm::vec2 getTangentAt(double offset) const;
		glm::vec2 getNormalAt(double offset) const;

		CurveLocation getLocationAtTime(double t) const;
		glm::vec2 getPointAtTime(double t) const;
		glm::vec2 getTangentAtTime(double t) const;
		glm::vec2 getNormalAtTime(double t)const;

		double Path::getNearestTime(glm::vec2 point) const;

		double getLength() const;

		std::shared_ptr<Curve> getFirstCurve() const;
		std::shared_ptr<Curve> getLastCurve() const;

		std::vector<std::shared_ptr<Segment>> getSegments() const;
		std::shared_ptr<Segment> getFirstSegment() const;
		std::shared_ptr<Segment> getLastSegment() const;

		void draw() const;

		// mutte Path
		void add(Segment segment);
		void add(std::shared_ptr<Segment> segment);
		void add(std::vector<std::shared_ptr<Segment>> segments);

		void insert(int index, std::shared_ptr<Segment> segment);
		void insert(int index, std::vector<std::shared_ptr<Segment>> segments);

		void _add(std::vector<std::shared_ptr<Segment>> segs, int index = -1);
		void _adjustCurves(int start, int end);


	};
}

