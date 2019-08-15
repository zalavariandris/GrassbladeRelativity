#pragma once
#include "ofMain.h"
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

	void showCurve() {
		static glm::vec2 A{ 0,0 };
		static glm::vec2 B{ 100, 100 };
		static glm::vec2 C{ 200, 200 };
		static glm::vec2 D{ 300, 300 };

		Im2D::DragBezierSegment("Curve", &A, &B, &C, &D);

		//
		Curve curve{ Segment(A, glm::vec2(), B-A), Segment(D, C-D, glm::vec2()) };
		glm::vec2 mousePos = Im2D::GetMousePos();

		// draw curve with  OF
		ofSetColor(ofColor::black);
		curve.draw();

		// draw normals
		ofSetColor(ofColor::green);
		for (int i = 0; i < 10; i++) {
			float t = (float)i / 9;
			glm::vec2 N = curve.getNormal(t);
			glm::vec2 P = curve.getPoint(t);
			ofDrawLine(P, P + N * 100);
		}

		// draw tangents
		ofSetColor(ofColor::red);
		for (int i = 0; i < 10; i++) {
			float t = (float)i / 9;
			glm::vec2 N = curve.getTangent(t);
			glm::vec2 P = curve.getPoint(t);
			ofDrawLine(P, P + N * 100);
		}
	}

	void showPath() {
		//
		static glm::vec2 A{ 30,75 };
		static glm::vec2 B{ 100,20 };
		static glm::vec2 C{ 170,75 };

		Im2D::DragPoint("A", &A);
		Im2D::DragPoint("B", &B);
		Im2D::DragPoint("C", &C);

		Path path;
		path.add(Segment(A));
		path.add(Segment(B, { -50, 0 }, { 50, 0 }));
		path.add(Segment(C));

		static float offset{ 0.5 };

		CurveLocation loc = path.getLocationAt(offset);
		cout << "--- path ---" << endl;
		cout << "curves count: " << path.getCurves().size() << endl;
		cout << loc.time << endl;
		//if(loc)
		//	Q = loc.curve.lock()->getPoint(loc.time);
		

		// draw path with OF
		ofSetColor(ofColor::black);
		path.draw();
	}

	void draw() {
		static float t{ 0 };
		ImGui::Begin("GrassbladeRelativity");
		Im2D::ViewerBegin("Viewer1");
		showCurve();
		//showPath();
		Im2D::ViewerEnd();
		ImGui::End();
	}

	void update() {

	}
};