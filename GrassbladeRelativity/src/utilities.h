#pragma once
#include "ofCamera.h"
#include "Paper/Segment.h"
#include "Paper/Curve.h"
#include "Paper/Path.h"
#include "Animation/AnimationCurve.h"
#include "Geo/Polygon.h"

/* utilities */
namespace Field {
	// Path
	glm::vec2 pathToRect(Paper::Path const & path, glm::vec2 uv);

	glm::vec2 rectToPath(Paper::Path const & path, glm::vec2 xy);

	glm::vec2 pathToPath(Paper::Path const & source, Paper::Path const & target, glm::vec2 P0);

	// Polygon
	glm::vec2 polyToRect(Geo::Polygon const & poly, glm::vec2 uv);

	glm::vec2 rectToPoly(Geo::Polygon const & poly, glm::vec2 xy);

	glm::vec2 polyToPoly(Geo::Polygon & source, Geo::Polygon & target, glm::vec2 P0);
}

Paper::Path extend(Paper::Path path, double length);

std::vector<glm::vec2> divide(Paper::Path const & path, int count, bool weighted = true);

namespace Utils{
	void ofSyncCameraToViewport(ofCamera & camera);

	void ofDraw(Paper::Segment segment);

	void ofDraw(Paper::Curve curve);

	void ofDraw(Paper::Path path, bool verbose = true);

	void addTrajectory(Animation::AnimationCurve const & x, Animation::AnimationCurve const & y, int from, int to);
}

