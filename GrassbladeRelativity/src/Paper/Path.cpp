#include "Path.h"

#include <algorithm> // for 'min' and 'max'
#include "numerical.h"
#include "CurveLocation.h"
#include <map>
namespace Paper {
	Path::Path() {
		mClosed = false;
	}

	Path::Path(std::vector<Segment> segments) {
		mClosed = false;
		_add(segments);
	}

	Path::Path(const Path & other) {
		//Copy constructor is called when a new object is created from an existing object, as a copy of the existing object

		mClosed = other.mClosed;
		mSegments = other.mSegments;
		for (auto & segment : mSegments) {
			segment.mPath = this;
		}
		mCurves = other.mCurves;
		for (auto & curve : mCurves) {
			curve.mPath = this;
		}
		CurvesNeedsUpdate = other.CurvesNeedsUpdate;
		_adjustCurves(0, mCurves.size());

		mLength = other.mLength;
		LengthNeedsUpdate = other.LengthNeedsUpdate;
	}

	Path& Path::operator = (const Path &other) {
		//And assignment operator is called when an already initialized object is assigned a new value from another existing object.
		auto newPath = Path();
		mClosed = other.mClosed;
		mSegments = other.mSegments;
		for (auto & segment : mSegments) {
			segment.mPath = this;
		}
		mCurves = other.mCurves;
		for (auto & curve : mCurves) {
			curve.mPath = this;
		}
		CurvesNeedsUpdate = true;
		_adjustCurves(0, mCurves.size());
		

		mLength = other.mLength;
		LengthNeedsUpdate = other.LengthNeedsUpdate;
		return newPath;
	}

	void Path::_adjustCurves(int start, int end) {
		for (auto i = start; i < end; i++) {
			auto & curve = mCurves.at(i);
			curve.mPath = this;
			curve.mSegment1 = &mSegments[i];
			curve.mSegment2 = mClosed ? &mSegments[0] : &mSegments[i + 1];
			//curve._changed();
		}
		// If it's the first segment, correct the last segment of closed
		// paths too:
		int _ = mClosed && !start ? mSegments.size() - 1 : start - 1;
		if (_ < mCurves.size() && _ >= 0) {
			auto & curve = mCurves[_];
			curve.mSegment2 = &mSegments[start >= 0 < mSegments.size() ? start : 0];
			//curve._changed();
		}
		// Fix the segment after the modified range, if it exists
		if (end < mCurves.size()) {
			auto & curve = mCurves[end];
			curve.mSegment1 = &mSegments[end];
			//curve._changed();
		}
	};
	//Path::Path(const Path & other) {
	//	// Deep copy
	//	CurvesNeedsUpdate = true;
	//	mSegments = std::vector<Segment>();
	//	for (auto const & segment : other.getSegments()) {
	//		add({
	//			Paper::Segment(
	//				segment.point(),
	//				segment.handleIn(),
	//				segment.handleOut()
	//			)
	//		});
	//	}
	//	LengthNeedsUpdate = true;
	//}

	std::vector<Segment> & Path::getSegments() {
		return mSegments;
	}

	Segment & Path::getFirstSegment() {
		return mSegments[0];
	}

	Segment& Path::getLastSegment() {
		return mSegments[mSegments.size() - 1];
	}

	const std::vector<Segment> & Path::getSegments() const{
		return mSegments;
	}

	const Segment & Path::getFirstSegment() const{
		return mSegments[0];
	}

	const Segment& Path::getLastSegment() const{
		return mSegments[mSegments.size() - 1];
	}

	int Path::countCurves() const {
		auto length = mSegments.size();
		// Reduce length by one if it's an open path:
		return !mClosed && length > 0 ? length - 1 : length;
	}

	std::vector<Curve> & Path::getCurves(){
		if (CurvesNeedsUpdate) {
			auto length = countCurves();
			mCurves = std::vector<Curve>(length);
			for (auto i = 0; i < length; i++) {
				// Use first segment for segment2 of closing curve
				mCurves[i] = Curve(&mSegments[i], (i + 1) < mSegments.size() ? &mSegments[i + 1] : &mSegments[0]);
				mCurves[i].mPath = this;
			}
			CurvesNeedsUpdate = false;
		}
		return mCurves;
	}

	const std::vector<Curve> & Path::getCurves() const{
		if (CurvesNeedsUpdate) {
			auto length = countCurves();
			mCurves = std::vector<Curve>();
			for (auto i = 0; i < length; i++) {
				// Use first segment for segment2 of closing curve
				auto curve = Curve(&mSegments[i], (i + 1) < mSegments.size() ? &mSegments[i + 1] : &mSegments[0]);
				curve.mPath = const_cast<Path*>(this);
				mCurves.push_back(curve);
			}
			CurvesNeedsUpdate = false;
		}
		return mCurves;
	}

	/*
		Returns the curve location of the specified offset on the path.
		TODO: handle negative values.
	*/
	CurveLocation Path::getLocationAt(double offset) const {
		if (offset < 0) {
			return CurveLocation();
		}

		auto curves = getCurves();
		double length = 0;
		for (auto i = 0; i < curves.size(); i++) {
			auto start = length;
			Curve & curve = curves[i];
			length += curve.getLength();
			if (length > offset) {
				// Found the segment within which the length lies
				return curve.getLocationAt(offset - start);
			}
		}

		// It may be that through imprecision of getLength, that the end of
		// the last curve was missed:
		if (curves.size() > 0 && offset <= getLength()) {
			return CurveLocation(curves[curves.size() - 1], 1.0);

		}

		//return invalid curve location
		return CurveLocation();
	}

	Curve & Path::getFirstCurve() {
		return getCurves()[0];
	}
	Curve & Path::getLastCurve() {
		auto & curves = getCurves();
		return curves[curves.size() - 1];
	}

	const Curve & Path::getFirstCurve() const {
		return getCurves()[0];
	}
	const Curve & Path::getLastCurve() const {
		auto & curves = getCurves();
		return curves[curves.size() - 1];
	}

	double Path::getNearestTime(glm::vec2 point) const {
		auto curves = getCurves();
		if (curves.size() <= 0)
			return NAN;
		double minDist = std::numeric_limits<double>::infinity();
		double minT{NAN};
		int index;
		for (auto i = 0; i < curves.size(); i++) {
			double t = curves[i].getNearestTime(point);
			glm::vec2 P = curves[i].getPointAtTime(t);
			//double distance = glm::distance(point, P);
			auto temp = point - P;
			double distance = glm::dot(temp, temp);
			if (distance < minDist) {
				minT = t;
				minDist = distance;
				index = i;
			}
		}
		return (minT + index) / curves.size();
	};

	CurveLocation Path::getLocationAtTime(double t) const {
		//
		if (t < 0.0 || t>1.0 || isnan(t))
			return CurveLocation();

		//
		auto const & curves = getCurves();

		//
		auto minT = Numerical::CURVETIME_EPSILON;
		auto maxT = 1 - minT;
		if (t < minT)
			return CurveLocation(getFirstCurve(), 0.0);

		if (t > maxT)
			return CurveLocation(getLastCurve(), 1.0);

		//
		int i = t * curves.size();

		double curveTime = t * curves.size() - i;
		return curves[i].getLocationAtTime(curveTime);
	}

	glm::vec2 Path::getPointAtTime(double t) const {
		CurveLocation loc = getLocationAtTime(t);
		if (loc)
			return loc.point();
		return glm::vec2(NAN);
	}

	glm::vec2 Path::getNormalAtTime(double t) const {
		CurveLocation loc = getLocationAtTime(t);
		if (loc)
			return loc.normal();
		return glm::vec2(NAN);
	}

	glm::vec2 Path::getTangentAtTime(double t) const {
		CurveLocation loc = getLocationAtTime(t);
		if (loc)
			return loc.tangent();
		return glm::vec2(NAN);
	}

	glm::vec2 Path::getPointAt(double offset) const {
		CurveLocation loc = getLocationAt(offset);
		if (loc)
			return loc.point();
		return glm::vec2(NAN);
	}

	glm::vec2 Path::getTangentAt(double offset) const {
		CurveLocation loc = getLocationAt(offset);
		if (loc)
			return loc.tangent();
		return glm::vec2(NAN);
	}

	glm::vec2 Path::getNormalAt(double offset) const {
		CurveLocation loc = getLocationAt(offset);
		return loc.normal();
	}

	double Path::getLength() const {
		if (LengthNeedsUpdate) {
			auto curves = getCurves();
			double length = 0;
			for (auto i = 0; i < curves.size(); i++) {
				length += curves[i].getLength();
			}
			mLength = length;
			LengthNeedsUpdate = false;
		}
		return mLength;
	}

	bool Path::closed() const {
		return mClosed;
	};
	void Path::closed(bool val) {
		mClosed = val;
		LengthNeedsUpdate = true;
		CurvesNeedsUpdate = true;
	}

	void Path::add(Segment segment) {
		_add({ Segment(segment) });
	}

	void Path::add(std::vector<Segment> segments) {
		_add(segments);
	}

	void Path::insert(int index, Segment segment) {
		_add({ segment }, index);
	}
	void Path::insert(int index, std::vector<Segment> segments) {
		_add(segments, index);
	}

	void Path::_add(std::vector<Segment> segs, int index) {
		// Local short-cuts:
		int amount = segs.size();
		bool append = index < 0;
		index = append ? mSegments.size() : index;

		// Scan through segments to add first, convert if necessary and set
		// _path and _index references on them.
		for (auto i = 0; i < amount; i++) {
			auto & segment = segs[i];
			// If the segments belong to another path already, clone them before adding:
			//if (segment._path != nullptr)
			//	segment = Segment(segment);// segment.clone();
			segment.mPath = const_cast<Path*>(this);
			segment.mIndex = index + i;
		}

		if (append) {
			mSegments.reserve(mSegments.size() + segs.size());
			mSegments.insert(mSegments.end(), segs.begin(), segs.end());
		}
		else {
			mSegments.reserve(mSegments.size() + segs.size());
			mSegments.insert(mSegments.begin() + index, segs.begin(), segs.end());
		}

		// Keep the curves list in sync all the time in case it was requested
		// already.
		if (!CurvesNeedsUpdate) {
			auto total = countCurves();
			// If we're adding a new segment to the end of an open path, we need to step one index down to get its curve.
			auto start = index > 0 && index + amount - 1 == total ? index - 1 : index;
			auto insert = start;
			auto end = std::min(start + amount, total);

			// Insert new curves, but do not initialize their segments yet, since #_adjustCurves() handles all that for us
			for (auto i = insert; i < end; i++) {
				mCurves.insert(mCurves.begin() + i, Curve(
					const_cast<Path*>(this),
					nullptr,
					nullptr
					)
				);
			}
			// Adjust segments for the curves before and after the removed ones
			_adjustCurves(start, end);
		}

		LengthNeedsUpdate = true;
	}
}