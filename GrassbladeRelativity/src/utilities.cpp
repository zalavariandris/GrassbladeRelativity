#pragma once
#include "utilities.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include "Im2D/Im2D.h"
#include "Im2D/im2d_draw.h" // addLineSegment
#include "ofGraphics.h"
#include "ofAppRunner.h" //ofGetWidth(), ofGetHeight()

Paper::Path extend(Paper::Path path, double length) {
	Paper::Path newPath{ path };
	auto firstLocation = newPath.getLocationAtTime(0);
	auto lastLocation = newPath.getLocationAtTime(1.0);
	newPath.insert(0, std::make_shared<Paper::Segment>(firstLocation._point - firstLocation._tangent*length));
	newPath.add(std::make_shared<Paper::Segment>(lastLocation._point + lastLocation._tangent*length));
	return newPath;
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
		for (auto segment : path.getSegments()) {
			ofDrawCircle(segment->point(), 2.0);
			ofDrawLine(segment->point(), segment->point() + segment->handleIn());
			ofDrawLine(segment->point(), segment->point() + segment->handleOut());
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
