#pragma once
#include "imgui.h"
#include "Im2D/Im2D.h"
#include "Paper/Path.h"
#include <glm/glm.hpp>
#include "Im2D/Im2D.h"
#include "Im2D/im2d_draw.h"

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
namespace {
	double lerp(double a, double b, double t) {
		return a + t * (b - a);
	};

	glm::vec2 lerp(glm::vec2 A, glm::vec2 B, double t) {
		return glm::vec2(lerp(A.x, B.x, t), lerp(A.y, B.y, t));
	}

	struct Key {
		bool selected{ false };
		double frame;
		glm::vec2 pos;
		Key(double frame, glm::vec2 pos) :frame(frame), pos(pos) {};
	};

	class AnimationCurve{
	private:
		vector<Key> _keys;

	public:
		bool selected{ false };
		AnimationCurve() {};
		AnimationCurve(std::vector<Key> keys) {
			setKeys(keys);
		};

		void setKeys(std::vector<Key> keys) {
			_keys = keys;
			std::sort(_keys.begin(), _keys.end(), [](auto A, auto B) {
				return A.frame < B.frame;
			});
		}

		glm::vec2 getValueAtFrame(double frame) const{
			if (_keys.empty())
				return glm::vec2();

			if (frame <= _keys[0].frame)
				return _keys[0].pos;

			if (frame >= _keys[_keys.size() - 1].frame)
				return _keys[_keys.size() - 1].pos;

			// find embace keys
			double i1 = 0;
			while (_keys[i1].frame <= frame)
				i1++;
			int i0 = i1 - 1;

			// extract values fro lerp
			glm::vec2 p0 = _keys[i0].pos;
			glm::vec2 p1 = _keys[i1].pos;
			double f0 = _keys[i0].frame;
			double f1 = _keys[i1].frame;
			double t = (frame - f0) / (f1 - f0);

			return lerp(p0, p1, t);
		}

		void setValueAtFrame(glm::vec2 value, double frame) {
			if (_keys.empty())
				_keys.push_back(Key(frame, value));

			if (frame < _keys[0].frame)
				_keys.insert(_keys.begin(), 1, Key(frame, value));

			if (frame > _keys[_keys.size() - 1].frame)
				_keys.push_back(Key(frame, value));

			// find key by frame
			auto i = -1;
			for (int i=0; i < _keys.size(); i++)
				if (getKeys()[i].frame == frame) {
					_keys[i].pos = value;
					return;
				}

			// find embace keys
			double i1 = 0;
			while (_keys[i1].frame <= frame)
				i1++;
			int i0 = i1 - 1;

			_keys.insert(_keys.begin() + i0, 1, Key(frame, value));
		}

		std::vector<Key> & getKeys(){
			return _keys;
		}
	};
}

void showGraphEditorDemo() {
	ImGui::Begin("GraphEditorDemo");
	static int F{ 0 };
	ImGui::SliderInt("frame", &F, -10, 110);

	AnimationCurve animCurve({
		Key(0,{0,600}),
		Key(5,{50,500}),
		Key(10,{100,100}),
		Key(15,{150,300}),
		Key(20,{200,100}),
		Key(25,{250,100})
	});

	//ImGui::PlotLines("X", [](void*data, int idx) {
	//	AnimationCurve animCurve({
	//		Key(0,{0,600}),
	//		Key(5,{50,500}),
	//		Key(10,{100,100}),
	//		Key(15,{150,300}),
	//		Key(20,{200,100}),
	//		Key(25,{250,100})
	//	});
	//	return animCurve.getValueAtFrame(idx).x;
	//}, NULL, 100, 0, "", 0, 600, ImVec2(300, 150));

	//ImGui::PlotLines("Y", [](void*data, int idx) {
	//	AnimationCurve animCurve({
	//		Key(0,{0,600}),
	//		Key(5,{50,500}),
	//		Key(10,{100,100}),
	//		Key(15,{150,300}),
	//		Key(20,{200,100}),
	//		Key(25,{250,100})
	//	});
	//	return animCurve.getValueAtFrame(idx).x;
	//}, NULL, 100, 0, "", 0, 600, ImVec2(300, 150));
	ImGui::End();

	
	glm::vec2 pos = animCurve.getValueAtFrame(F);
	pos.y = ofGetHeight() - pos.y;
	ofDrawCircle(pos, 50);
}

void showAnimationDemo() {
	// Model
	static glm::vec2 P;
	static int F{ 0 };
	static int begin{ 0 };
	static int end{ 50 };
	static bool play{ false };
	static AnimationCurve animCurve({
		Key(0,{0,250}),
		Key(5,{50,200}),
		Key(10,{100,50}),
		Key(15,{150,100}),
		Key(20,{200,50}),
		Key(25,{250,50})
	});

	// Logic
	if (play)
		F++;
	if (F > end)
		F = begin;
	if (F < begin)
		F = end;

	P = animCurve.getValueAtFrame(F);

	// GUI
	ImGui::Begin("Viewer");
	{
		Im2D::ViewerBegin("viewer");
		if (Im2D::DragPoint("P", &P)) {
			animCurve.setValueAtFrame(P, F);
		}
		Im2D::ViewerEnd();
	}ImGui::End();

	ImGui::Begin("Timeline");
	{
		// time slider
		ImGui::PushItemWidth(50);
		ImGui::DragInt("##begin", &begin);
		if (begin >= end)
			begin = end;
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() - 2 * (ImGui::GetWindowContentRegionWidth() - ImGui::GetContentRegionAvailWidth()));
		ImGui::SliderInt("##frame", &F, begin, end);
		ImGui::SameLine();
		ImGui::PushItemWidth(50);
		ImGui::DragInt("##end", &end);
		if (end <= begin)
			end = begin;

		// time controls
		auto style = ImGui::GetStyle();
		float btnWidth = 25;
		ImGui::SetCursorPosX(ImGui::GetContentRegionAvailWidth()/2-(btnWidth *6+2*style.ItemSpacing.x)/2);
		ImGui::BeginGroup();
		if (ImGui::Button("|<<", ImVec2(btnWidth, 20)))
			F = begin;
		ImGui::SameLine();
		if (ImGui::Button("|<", ImVec2(btnWidth, 20)))
			F--;
		ImGui::SameLine();
		if (ImGui::Button("||", ImVec2(btnWidth, 20)))
			play = false;
		ImGui::SameLine();
		if (ImGui::Button(">", ImVec2(btnWidth,20)))
			play = true;
		ImGui::SameLine();
		if (ImGui::Button(">|", ImVec2(btnWidth, 20)))
			F++;
		ImGui::SameLine();
		if (ImGui::Button(">>|", ImVec2(btnWidth, 20)))
			F = end;
		ImGui::EndGroup();

	}ImGui::End();

	ImGui::Begin("DopeSheet");
	{
		if (ImGui::Button("Delete")) {
			auto & keys = animCurve.getKeys();
			for (int i = keys.size() - 1; i >= 0; i--) {
				if (keys.at(i).selected) {
					cout << "erease key at: " << i << endl;
					keys.erase(keys.begin() + i);
				}
			}
			// collect indices
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted("delete selected keys");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
		for (auto i = 0; i < animCurve.getKeys().size(); i++) {
			auto & key = animCurve.getKeys().at(i);
			ImGui::SetCursorPosX(key.frame*30);
			ImGui::PushID(i);
			if (ImGui::Selectable("K", key.selected, 0, ImVec2(30, 30))) {
				key.selected = !key.selected;
			}
			ImGui::PopID();
			ImGui::SameLine();
		}
		ImGui::NewLine();

	}ImGui::End();
	
	ImGui::Begin("Plot"); 
	{
		// convert keys to data;
		std::vector<float> plotX(end+1 - begin);
		std::vector<float> plotY(end+1- begin);
		for (auto f = begin; f < end+1; f++) {
			auto pos = animCurve.getValueAtFrame(f);
			plotX[f-begin] = pos.x;
			plotY[f-begin] = pos.y;
		}
		float scale_min = -600, scale_max = 600;
		ImGui::PlotLines("x", plotX.data(), plotX.size(), 0, "posX", scale_min, scale_max, ImVec2(300, 150));
		ImGui::PlotLines("y", plotY.data(), plotY.size(), 0, "posY", scale_min, scale_max, ImVec2(300, 150));
	}ImGui::End();

	ImGui::Begin("GraphEditor");
	{
		Im2D::ViewerBegin("viewport", ImVec2(), Im2DViewportFlags_AllowNonUniformZoom);
		// draw anim curve
		auto & keys = animCurve.getKeys();
		for (auto i = 0; i < keys.size()-1; i++) {
			Key keyA = keys.at(i);
			Key keyB = keys.at(i+1);

			// draw x curve
			glm::vec2 posAx = glm::vec2(keyA.frame*10, keyA.pos.x );
			glm::vec2 posBx = glm::vec2(keyB.frame*10, keyB.pos.x);

			addLineSegment(posAx, posBx, ImColor(255,0,0));

			// draw y curve
			glm::vec2 posAy = glm::vec2(keyA.frame * 10, keyA.pos.y);
			glm::vec2 posBy = glm::vec2(keyB.frame * 10, keyB.pos.y);
			addLineSegment(posAy, posBy, ImColor(0, 255, 0));
		}
		for (auto & key : keys) {
			ImGui::PushID(&key);
			glm::vec2 Cx = glm::vec2(key.frame * 10, key.pos.x);
			if (Im2D::DragPoint("##Cx", &Cx, 4.0)) {
				key.frame = Cx.x / 10;
				key.pos.x = Cx.y;
			}
			glm::vec2 Cy = glm::vec2(key.frame * 10, key.pos.y);
			if (Im2D::DragPoint("##Cy", &Cy, 4.0)) {
				key.frame = Cy.x / 10;
				key.pos.y = Cy.y;
			}
			ImGui::PopID();
		}

		// 
		addLineSegment({ F * 10,0 }, { F * 10,500 }, ImColor(255,255,0, 128));
		Im2D::ViewerEnd();
	}ImGui::End();
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