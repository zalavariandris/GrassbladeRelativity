#pragma once
#include "Paper/Path.h"
#include "ofCamera.h"
#include "of3dGraphics.h"
#include "Im2D/Im2D.h"

void showCurveDemo() {
	static ofCamera cam;
	cam.enableOrtho();
	cam.setNearClip(-1000);
	cam.setFarClip(1000);
	cam.setPosition(0, 0, 0);

	static glm::vec2 A{ 10,    0 };
	static glm::vec2 B{ -60, -70 };
	static glm::vec2 C{ -60,-140 };
	static glm::vec2 D{ 10, -180 };

	static glm::vec2 P{ -100, -140 };

	ImGui::SetNextWindowBgAlpha(0.0);
	Im2D::ViewerBegin("viewport");
	Im2D::DragPoint("A", &A);
	Im2D::DragPoint("B", &B);
	Im2D::DragPoint("C", &C);
	Im2D::DragPoint("D", &D);

	Im2D::DragPoint("P", &P);

	auto curve = std::make_shared<Curve>(
		std::make_shared<Segment>(A, glm::vec2(), B - A),
		std::make_shared<Segment>(D, C - D, glm::vec2())
		);

	double t = curve->getNearestTime(P);
	glm::vec2 Q = curve->getPointAtTime(t);

	ofSyncCameraToViewport(cam);
	Im2D::ViewerEnd();

	cam.begin();
	ofSetColor(ofColor::black);
	ofDraw(*curve);
	ofDraw(*(curve->_segment1));
	ofDraw(*(curve->_segment2));
	ofSetColor(ofColor::white);
	ofDrawCircle(Q, 10);
	ofDrawLine(P, Q);
	cam.end();
}

void showPathDemo() {
	static ofCamera cam;
	cam.enableOrtho();
	cam.setNearClip(-1000);
	cam.setFarClip(1000);
	cam.setPosition(0, 0, 0);

	static glm::vec2 A{ 0,  200 };
	static glm::vec2 B{ -60,  140 };
	static glm::vec2 C{ -100,   60 };
	static glm::vec2 D{ -100,    0 };
	static glm::vec2 E{ -100, -60 };
	static glm::vec2 F{ -60, -140 };
	static glm::vec2 G{ 0, -200 };

	static glm::vec2 P{ 30, 60 };

	ImGui::SetNextWindowBgAlpha(0.0);
	Im2D::ViewerBegin("viewport");

	Im2D::DragPoint("P", &P);
	static Path path = Path({
		std::make_shared<Segment>(A, glm::vec2(), B - A),
		std::make_shared<Segment>(D, C - D, E - D),
		std::make_shared<Segment>(G, F - G, glm::vec2())
		});

	Im2D::DragSegment("S0", path.getSegments()[0].get(), "Smooth");
	Im2D::DragSegment("S1", path.getSegments()[1].get(), "Smooth");
	Im2D::DragSegment("S2", path.getSegments()[2].get(), "Smooth");

	double t = path.getNearestTime(P);
	ImGui::Text("time: %f", (float)t);
	glm::vec2 Q = path.getPointAtTime(t);

	ofSyncCameraToViewport(cam);
	Im2D::ViewerEnd();

	cam.begin();
	ofSetColor(ofColor::black);
	ofDraw(path);
	ofSetColor(ofColor::white);
	ofDrawCircle(Q, 10);
	ofDrawLine(P, Q);
	cam.end();
}
