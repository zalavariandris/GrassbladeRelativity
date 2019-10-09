#pragma once
#include "ofCamera.h"
#include "Paper/Segment.h"
#include "Paper/Curve.h"
#include "Paper/Path.h"
#include "Animation/AnimationCurve.h"


/* utilities */
namespace Utils{
	void ofSyncCameraToViewport(ofCamera & camera);

	void ofDraw(Paper::Segment segment);

	void ofDraw(Paper::Curve curve);

	void ofDraw(Paper::Path path, bool verbose = true);

	void addTrajectory(Animation::AnimationCurve const & x, Animation::AnimationCurve const & y, int from, int to);
}