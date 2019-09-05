#pragma once
#include "ofCamera.h"
#include "Paper/Segment.h"
#include "Paper/Curve.h"
#include "Paper/Path.h"
#include "Animation/AnimCurve.h"


/* utilities */
namespace Utils{
	void ofSyncCameraToViewport(ofCamera & camera);

	void ofDraw(Paper::Segment segment);

	void ofDraw(Paper::Curve curve);

	void ofDraw(Paper::Path path, bool verbose = true);

	void addTrajectory(Animation::AnimCurve const & x, Animation::AnimCurve const & y, int from, int to);
}