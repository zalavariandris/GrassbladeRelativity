#pragma once

#include "imgui.h"
#include "Im2D/Im2D.h"
#include "Im2D/im2d_draw.h"

#include "Paper/Curve.h"
#include "Paper/Path.h"
class GrassbladeRelativity
{
public:
	GrassbladeRelativity() {

	}
	~GrassbladeRelativity() {

	}

	glm::vec2 curveToRect(Curve curve, glm::vec2 uv) {
		
		double distance = uv.x;
		double t = uv.y;

		glm::vec2 P = curve.getPoint(t);
		glm::vec2 normal = curve.getNormal(t, true);
		
		glm::vec2 xy = P + normal*distance;
		return xy;
	}

	glm::vec2 rectToCurve(Curve curve, glm::vec2 xy) {
		double t = curve.getNearestTime(xy);
		auto Q = curve.getPoint(t);
		double distance = glm::distance(xy, Q);

		glm::vec2 P = curve.getPoint(t);
		glm::vec2 normal = curve.getNormal(t);
		double dot = glm::dot(glm::normalize(xy-P), glm::normalize(normal));


		glm::vec2 uv(dot > 0 ? distance : -distance, t);
		return uv;
	}

	void setup() {

	}

	void draw() {
		static float t{ 0 };
		ImGui::Begin("GrassbladeRelativity");

		Im2D::ViewerBegin("Viewer1");

		static glm::vec2 A{ 0,0 };
		static glm::vec2 B{ 100, 100 };
		static glm::vec2 C{ 200, 200 };
		static glm::vec2 D{ 300, 300 };

		Im2D::DragBezierSegment("Curve", &A, &B, &C, &D);
		glm::vec2 mousePos = Im2D::GetMousePos();

		//
		Curve curve{ A, B, C, D };
		auto uv = rectToCurve(curve, mousePos);
		auto Q = curveToRect(curve, uv);
		Im2D::DragPoint("Q", &Q);

		//
		Path path;
		path.add(glm::vec2(30, 75));
		path.add(Segment(glm::vec2(100, 20), glm::vec2(-50, 0), glm::vec2(50, 0)));
		path.add(glm::vec2(170, 75));

		Im2D::ViewerEnd();
		ImGui::End();

		ofSetColor(ofColor::black);
		curve.draw();
		path.draw();

		ofSetColor(ofColor::green);
		for (int i = 0; i < 10; i++) {
			float t = (float)i / 9;
			glm::vec2 N = curve.getNormal(t);
			glm::vec2 P = curve.getPoint(t);
			ofDrawLine(P, P + N * 100);
		}

		ofSetColor(ofColor::red);
		for (int i = 0; i < 10; i++) {
			float t = (float)i / 9;
			glm::vec2 N = curve.getTangent(t);
			glm::vec2 P = curve.getPoint(t);
			ofDrawLine(P, P + N * 100);
		}
	}

	void update() {

	}
};