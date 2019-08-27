#pragma once
#include "imgui.h"
#include "Im2D/Im2D.h"
#include "Paper/Path.h"
#include <glm/glm.hpp>

/* utilities */
namespace {
	void syncCameraToViewport(ofCamera & camera) {
		float zoom = Im2D::getZoom();
		glm::vec2 pan = Im2D::getPan();
		glm::vec2 windowPos{ ImGui::GetWindowPos().x, ImGui::GetWindowPos().y };
		glm::vec2 windowSize = { ImGui::GetWindowWidth(), ImGui::GetWindowHeight() };
		glm::vec2 ofSize{ ofGetWidth(), ofGetHeight() };
		camera.setVFlip(true);
		camera.setScale(1.0 / zoom, 1.0 / zoom, 1.0 / zoom);
		camera.setGlobalPosition({ -pan - windowPos / zoom - windowSize / 2 / zoom + ofSize / 2 / zoom, 0 });
	}

	void ofDraw(Segment segment) {
		ofSetColor(ofColor::grey);
		ofDrawCircle(segment._point, 10);
		ofDrawCircle(segment._point + segment._handleIn, 5);
		ofDrawCircle(segment._point + segment._handleOut, 5);
		ofSetColor(ofColor(128, 128, 128, 128));
		ofDrawLine(segment._point, segment._point + segment._handleIn);
		ofDrawLine(segment._point, segment._point + segment._handleOut);
	}

	void ofDraw(Curve curve) {
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

	void ofDraw(Path path) {
		auto segments{ 32 * path.getCurves().size() };
		for (auto i = 0; i < segments; i++) {
			double t1 = (double)i / segments;
			double t2 = ((double)i + 1.0) / segments;

			glm::vec2 P1 = path.getPointAtTime(t1);
			glm::vec2 P2 = path.getPointAtTime(t2);
			ofDrawLine(P1, P2);
		};
	}

}

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

	syncCameraToViewport(cam);
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

	syncCameraToViewport(cam);
	Im2D::ViewerEnd();

	cam.begin();
	ofSetColor(ofColor::black);
	ofDraw(path);
	ofSetColor(ofColor::white);
	ofDrawCircle(Q, 10);
	ofDrawLine(P, Q);
	cam.end();
}

void showReadDemo() {
	//auto file = ofToDataPath("framecounter.mov");
	auto file = "C:/Users/andris/Desktop/grassfieldwind.mov";
	static Reader reader(file, false);

	ImGui::Text("file: %s", !reader.getFile().empty() ? reader.getFile().c_str() : "-no file-");
	
	ImGui::Text("dimension: %ix%ipx", reader.getWidth(), reader.getHeight());
	ImGui::Separator();

	int start{ 0 };
	int end{ reader.getFrameCount()-1 };
	static int F{ 0 };
	float fullWidth = ImGui::GetWindowContentRegionWidth();
	ImGui::PushItemWidth(50);
	ImGui::DragInt("##start", &start);
	ImGui::SameLine();
	ImGui::PushItemWidth(fullWidth-2*(fullWidth-ImGui::GetContentRegionAvailWidth()));
	ImGui::SliderInt("##frame", &F, start, end);
	ImGui::SameLine();
	ImGui::PushItemWidth(50);
	ImGui::DragInt("##end", &end);
	static bool play{ false };
	ImGui::Checkbox("Play", &play);
	if (play)
		F++;
	if (F >= reader.getFrameCount())
		F = 0;

	ofSetColor(ofColor::white);
	int border = 0;
	ofImage img = reader.getImageAtFrame(F);
	img.draw(border, border, ofGetWidth() - border * 2, ofGetHeight() - border * 2);

	ImGui::Separator();
	ImGui::Text("%.0ffps", ofGetFrameRate());
}

// GraphEditor demo
double lerp(double a, double b, double t) {
	return a + t * (b - a);
};

glm::vec2 lerp(glm::vec2 A, glm::vec2 B, double t) {
	return glm::vec2(lerp(A.x, B.x, t), lerp(A.y, B.y, t));
}

struct Key {
	double frame;
	glm::vec2 pos;
	Key(double frame, glm::vec2 pos) :frame(frame), pos(pos) {};
};

glm::vec2 getValueAtFrame(vector<Key> keys, double frame) {
	if (frame <= keys[0].frame)
		return keys[0].pos;

	if (frame >= keys[keys.size() - 1].frame)
		return keys[keys.size() - 1].pos;

	// find embace keys
	int i1 = 0;
	while (keys[i1].frame <= frame)
		i1++;
	int i0 = i1 - 1;

	// extract values fro lerp
	glm::vec2 p0 = keys[i0].pos;
	glm::vec2 p1 = keys[i1].pos;
	double f0 = keys[i0].frame;
	double f1 = keys[i1].frame;
	double t = (frame - f0) / (f1 - f0);

	return lerp(p0, p1, t);
};


void showGraphEditorDemo() {
	ImGui::Begin("GraphEditorDemo");
	static int F{ 0 };
	ImGui::SliderInt("frame", &F, -10, 110);

	vector<Key> keys{ 
		Key(0,{0,600}), 
		Key(5,{50,500}), 
		Key(10,{100,100}),
		Key(15,{150,300}),
		Key(20,{200,100}),
		Key(25,{250,100})
	};

	ImGui::PlotLines("X", [](void*data, int idx) {
		vector<Key> keys{
			Key(0,{0,600}),
			Key(5,{50,500}),
			Key(10,{100,100}),
			Key(15,{150,300}),
			Key(20,{200,100}),
			Key(25,{250,100})
		};
		return getValueAtFrame(keys, idx).x;
	}, NULL, 100, 0, "", 0, 600, ImVec2(300, 150));

	ImGui::PlotLines("Y", [](void*data, int idx) {
		vector<Key> keys{
			Key(0,{0,600}),
			Key(5,{50,500}),
			Key(10,{100,100}),
			Key(15,{150,300}),
			Key(20,{200,100}),
			Key(25,{250,100})
		};
		return getValueAtFrame(keys, idx).y;
	}, NULL, 100, 0, "", 0, 600, ImVec2(300, 150));
	ImGui::End();

	
	glm::vec2 pos = getValueAtFrame(keys, F);
	pos.y = ofGetHeight() - pos.y;
	ofDrawCircle(pos, 50);
}

void showDemos() {
	if (ImGui::Begin("ReadDemo")) {
		showReadDemo();
	}ImGui::End();

	ImGui::SetNextWindowBgAlpha(0.0);
	if (ImGui::Begin("CurveDemo")) {
		showCurveDemo();
	}ImGui::End();

	ImGui::SetNextWindowBgAlpha(0.0);
	if (ImGui::Begin("PathDemo")) {
		showPathDemo();
	}ImGui::End();

	
}