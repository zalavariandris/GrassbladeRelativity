#pragma once
#include "utilities.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include "Im2D/Im2D.h"
#include "Im2D/im2d_draw.h" // addLineSegment
#include "ofGraphics.h"
#include "ofAppRunner.h" //ofGetWidth(), ofGetHeight()

#include "Geo/Polygon.h"

Paper::Path extend(Paper::Path path, double length) {
	auto firstLocation = path.getLocationAtTime(0);
	auto lastLocation = path.getLocationAtTime(1.0);
	path.insert(0, Paper::Segment(firstLocation.point() - firstLocation.tangent()*length));
	path.add(Paper::Segment(lastLocation.point() + lastLocation.tangent()*length));
	return path;
}

std::vector<glm::vec2> divide(Paper::Path const & path, int count, bool weighted) {
	std::vector<glm::vec2> points;
	double length = path.getLength();
	points.reserve(count);
	for (auto i = 0; i <= count; i++) {
		points.push_back(weighted ? path.getPointAt(double(i)/count * length) : path.getPointAtTime((double)i / count));
	}
	return points;
}

namespace Field {
	glm::vec2 pathToRect(Paper::Path const & path, glm::vec2 uv) {
		double distance = uv.x;
		double t = uv.y;

		auto loc = path.getLocationAtTime(t);
		glm::vec2 P = loc.point();
		glm::vec2 normal = loc.normal();

		glm::vec2 xy = P + normal * distance;
		return xy;
	}

	glm::vec2 rectToPath(Paper::Path const & path, glm::vec2 xy) {
		double t = path.getNearestTime(xy);
		
		auto loc = path.getLocationAtTime(t);
		glm::vec2 P = loc.point();
		glm::vec2 normal = loc.normal();

		double distance = glm::distance(xy, P);
		double dot = glm::dot(glm::normalize(xy - P), glm::normalize(normal));
		distance = dot > 0 ? distance : -distance;
		glm::vec2 uv(distance, t);
		return uv;
	}

	glm::vec2 pathToPath(Paper::Path const & source, Paper::Path const & target, glm::vec2 P0) {
		return pathToRect(target, rectToPath(source, P0));
	}

	// Polygon
	glm::vec2 polyToRect(Geo::Polygon const & poly, glm::vec2 uv) {
		double distance = uv.x;
		double t = uv.y;

		glm::vec2 P = poly.getPointAtTime(t);
		glm::vec2 normal = poly.getNormalAtIndex(t*poly.getVertices().size());

		glm::vec2 xy = P + normal * distance;
		return xy;
	}

	glm::vec2 rectToPoly(Geo::Polygon const & poly, glm::vec2 xy) {
		double t = (double)poly.getNearestIndex(xy) / poly.getVertices().size();

		double offset = t * poly.getLength();
		glm::vec2 P = poly.getPointAt(t*poly.getLength());
		glm::vec2 normal = poly.getNormalAtIndex(t*poly.getVertices().size());

		double distance = glm::distance(xy, P);
		double dot = glm::dot(glm::normalize(xy - P), glm::normalize(normal));
		distance = dot > 0 ? distance : -distance;
		glm::vec2 uv(distance, t);
		return uv;
	}

	glm::vec2 polyToPoly(Geo::Polygon & source, Geo::Polygon & target, glm::vec2 P0) {
		return polyToRect(target, rectToPoly(source, P0));
	}
}

void Utils::ofSyncCameraToViewport(ofCamera & camera) {
	// set ortho params
	camera.enableOrtho();
	camera.setNearClip(-1000);
	camera.setFarClip(1000);
	camera.setPosition(0, 0, 0);

	// TODO: support non ufiform zoom
	float zoom = Im2D::getZoom().x;
	glm::vec2 pan = Im2D::getPan();
	glm::vec2 windowPos{ ImGui::GetWindowPos().x, ImGui::GetWindowPos().y };
	glm::vec2 windowSize = { ImGui::GetWindowWidth(), ImGui::GetWindowHeight() };
	glm::vec2 ofSize{ ofGetWidth(), ofGetHeight() };
	camera.setVFlip(true);
	camera.setScale(1.0 / zoom, 1.0 / zoom, 1.0 / zoom);
	glm::vec2 pos2D = -pan - windowPos / zoom - windowSize /2 / zoom + ofSize /2 / zoom;
	camera.setGlobalPosition({ pos2D, 0 });
}

void Utils::ofDraw(Paper::Segment segment) {
	ofSetColor(ofColor::grey);
	ofDrawCircle(segment.point(), 10);
	ofDrawCircle(segment.point() + segment.handleIn(), 5);
	ofDrawCircle(segment.point() + segment.handleOut(), 5);
	ofSetColor(ofColor(128, 128, 128, 128));
	ofDrawLine(segment.point(), segment.point() + segment.handleIn());
	ofDrawLine(segment.point(), segment.point() + segment.handleOut());
}

void Utils::ofDraw(Paper::Curve curve) {
	ofSetColor(ofColor::white);
	auto segments{ 8 };
	for (auto i = 0; i < segments; i++) {
		double t1 = (double)i / segments;
		double t2 = ((double)i + 1.0) / segments;

		glm::vec2 P1 = curve.getPointAtTime(t1);
		glm::vec2 P2 = curve.getPointAtTime(t2);
		ofDrawLine(P1, P2);
	};
}

void Utils::ofDraw(Paper::Path path, bool verbose) {
	auto segments{ 32 * path.getCurves().size() };
	for (auto i = 0; i < segments; i++) {
		double t1 = (double)i / segments;
		double t2 = ((double)i + 1.0) / segments;

		glm::vec2 P1 = path.getPointAtTime(t1);
		glm::vec2 P2 = path.getPointAtTime(t2);
		ofDrawLine(P1, P2);
	};

	if (verbose) {
		for (auto const & segment : path.getSegments()) {
			ofDrawCircle(segment.point(), 2.0);
			ofDrawLine(segment.point(), segment.point() + segment.handleIn());
			ofDrawLine(segment.point(), segment.point() + segment.handleOut());
		}
	}
}

void Utils::addTrajectory(Animation::AnimationCurve const & x, Animation::AnimationCurve const & y, int from, int to) {
	for (auto f = from; f < to; f++) {
		glm::vec2 A = glm::vec2(x.getValueAtFrame(f), y.getValueAtFrame(f));
		glm::vec2 B = glm::vec2(x.getValueAtFrame(f + 1), y.getValueAtFrame(f + 1));
		addLineSegment(A, B, ImColor(128, 128, 128, 128));
	}
}
