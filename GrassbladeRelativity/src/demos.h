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
		// TODO: support non ufiform zoom
		float zoom = Im2D::getZoom().x;
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
	bool selected{ false };
	int frame;
	double value;
	Key(int frame, double value) :frame(frame), value(value) {};
};

class AnimCurve{
private:
	vector<Key> _keys;
		

public:
	ImColor color{ 255,255,255,128 };
	AnimCurve() {};
	AnimCurve(std::vector<Key> keys) {
		setKeys(keys);
	};

	void setKeys(std::vector<Key> keys) {
		_keys = keys;
		std::sort(_keys.begin(), _keys.end(), [](auto A, auto B) {
			return A.frame < B.frame;
		});
	}

	double getValueAtFrame(double frame) const{
		if (_keys.empty())
			return 0.0;

		if (frame <= _keys[0].frame)
			return _keys[0].value;

		if (frame >= _keys[_keys.size() - 1].frame)
			return _keys[_keys.size() - 1].value;

		// find embace keys
		double i1 = 0;
		while (_keys[i1].frame <= frame)
			i1++;
		int i0 = i1 - 1;

		// extract values fro lerp
		double v0 = _keys[i0].value;
		double v1 = _keys[i1].value;
		int f0 = _keys[i0].frame;
		int f1 = _keys[i1].frame;
		double t = (frame - f0) / (f1 - f0);

		return lerp(v0, v1, t);
	}

	void insertKeyAtFrame(int frame) {
		double val = getValueAtFrame(frame);
		setValueAtFrame(val, frame);
	}

	void setValueAtFrame(double value, int frame) {
		if (_keys.empty())
			_keys.push_back(Key(frame, value));

		if (frame < _keys[0].frame)
			_keys.insert(_keys.begin(), 1, Key(frame, value));

		if (frame > _keys[_keys.size() - 1].frame)
			_keys.push_back(Key(frame, value));

		// find key at frame
		auto i = -1;
		for (int i=0; i < _keys.size(); i++)
			if (getKeys()[i].frame == frame) {
				_keys[i].value = value;
				return;
			}

		// find embace keys
		int i1 = 0;
		while (_keys[i1].frame <= frame)
			i1++;
		int i0 = i1 - 1;

		_keys.insert(_keys.begin() + i1, 1, Key(frame, value));
	}

	int getKeyIndexAtFrame(double frame) {
		for (int i = 0; i < _keys.size(); i++){
			if (getKeys()[i].frame == frame) {
				return i;
			}
		}
		return -1;
	}

	bool hasKeyAtFrame(double frame) {
		return getKeyIndexAtFrame(frame) >= 0;
	}

	bool removeKeyAtFrame(double frame) {
		auto idx = getKeyIndexAtFrame(frame);
		if (idx < 0)
			return false;

		auto & keys = getKeys();
		keys.erase(keys.begin()+idx);
		return true;
	}

	std::vector<Key> & getKeys(){
		return _keys;
	}
};



void showAnimationDemo() {
	// Model
	static glm::vec2 P;
	static int F{ 0 };
	static int begin{ 0 };
	static int end{ 500 };
	static bool play{ false };
	static AnimCurve animCurveX({
		Key(0,0),
		Key(50,50),
		Key(100,100),
		Key(150,150),
		Key(200,200),
		Key(250,250)
	});
	animCurveX.color = ImColor(255, 0, 0);
	static AnimCurve animCurveY({
		Key(0,250),
		Key(50,200),
		Key(100,50),
		Key(150,100),
		Key(200,50),
		Key(250,50)
	});
	animCurveY.color = ImColor(0, 255, 0);

	// viewer options
	static int framesBefore{ 100 };
	static int framesAfter{ 100 };
	ImGui::DragInt("onionBefore", &framesBefore);
	ImGui::DragInt("onionAfter", &framesAfter);

	// Logic
	if (play) {
		F++;
		if (F > end)
			F = begin;
		if (F < begin)
			F = end;
	}

	P = {
		animCurveX.getValueAtFrame(F),
		animCurveY.getValueAtFrame(F)
	};

	// GUI
	ImGui::Begin("Viewer");
	{
		Im2D::ViewerBegin("viewer");
		if (Im2D::DragPoint("P", &P)) {
			animCurveX.setValueAtFrame(P.x, F);
			animCurveY.setValueAtFrame(P.y, F);
		}

		// draw trajectory
		for (auto f = F-framesBefore; f < F+framesAfter; f++) {
			glm::vec2 A = glm::vec2(animCurveX.getValueAtFrame(f), animCurveY.getValueAtFrame(f));
			glm::vec2 B = glm::vec2(animCurveX.getValueAtFrame(f+1), animCurveY.getValueAtFrame(f+1));
			addLineSegment(A, B, ImColor(128, 128, 128, 128));
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
		if (ImGui::Button("|<<", ImVec2(btnWidth, 25)))
			F = begin;
		ImGui::SameLine();
		if (ImGui::Button("|<", ImVec2(btnWidth, 25)))
			F--;
		ImGui::SameLine();
		if (ImGui::Button("||", ImVec2(btnWidth, 25)))
			play = false;
		ImGui::SameLine();
		if (ImGui::Button(">", ImVec2(btnWidth,25)))
			play = true;
		ImGui::SameLine();
		if (ImGui::Button(">|", ImVec2(btnWidth, 25)))
			F++;
		ImGui::SameLine();
		if (ImGui::Button(">>|", ImVec2(btnWidth, 25)))
			F = end;
		ImGui::EndGroup();

	}ImGui::End();

	ImGui::Begin("DopeSheet");
	{
		if (ImGui::Button("Delete")) {
			auto & keys = animCurveX.getKeys();
			for (int i = keys.size() - 1; i >= 0; i--) {
				if (keys.at(i).selected) {
					keys.erase(keys.begin() + i);
				}
			}
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted("delete selected keys");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
		for (auto i = 0; i < animCurveX.getKeys().size(); i++) {
			auto & key = animCurveX.getKeys().at(i);
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
			plotX[f-begin] = animCurveX.getValueAtFrame(f);
			plotY[f-begin] = animCurveY.getValueAtFrame(f);
		}
		float scale_min = -600, scale_max = 600;
		ImGui::PlotLines("x", plotX.data(), plotX.size(), 0, "posX", scale_min, scale_max, ImVec2(300, 150));
		ImGui::PlotLines("y", plotY.data(), plotY.size(), 0, "posY", scale_min, scale_max, ImVec2(300, 150));
	}ImGui::End();

	ImGui::Begin("GraphEditor");
	{
		
		std::vector<AnimCurve*> curves;
		if (ImGui::Button("Insert Key At current frame")) {
			for (AnimCurve * animCurve : curves) {
				animCurve->insertKeyAtFrame(F);
			}
		}
		if (ImGui::Button("Delete Selected Keys")) {
			for (AnimCurve * animCurve : curves) {
				auto & keys = animCurve->getKeys();
				for (int i = keys.size() - 1; i >= 0; i--) {
					if (keys.at(i).selected) {
						keys.erase(keys.begin() + i);
					}
				}
			}
		}
		Im2D::ViewerBegin("viewport", ImVec2(), Im2DViewportFlags_Grid | Im2DViewportFlags_AllowNonUniformZoom);

		// Handle selection
		static bool isSelecting{ false };
		static glm::vec2 selectionFrom;
		static glm::vec2 selectionTo;
		if (ImGui::IsItemActivated() && !ImGui::GetIO().KeyAlt && !ImGui::GetIO().KeyCtrl) {
			isSelecting = true;
			selectionFrom.x = ImGui::GetMousePos().x;
			selectionFrom.y = ImGui::GetMousePos().y;
			selectionTo.x = selectionFrom.x;
			selectionTo.y = selectionFrom.y;
		}
		if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0) && !ImGui::GetIO().KeyAlt && !ImGui::GetIO().KeyCtrl) {
			selectionTo.x = ImGui::GetMousePos().x;
			selectionTo.y = ImGui::GetMousePos().y;
		}
		if (ImGui::IsItemDeactivated()) {
			isSelecting = false;
		}

		if (isSelecting) {
			// select keys
			Rect selectionRect(selectionFrom, selectionTo);
			for (AnimCurve * animCurve : curves) {
				for (auto & key : animCurve->getKeys()) {
					glm::vec2 P = toScreen({ key.frame, key.value });
					if (selectionRect.contains(P)) {
						key.selected = true;
					}
					else {
						key.selected = false;
					}
				}
			}

			// draw selection rect
			auto window = ImGui::GetCurrentWindow();
			window->DrawList->AddRectFilled(
				ImVec2(selectionRect.left(), selectionRect.top()),
				ImVec2(selectionRect.right(), selectionRect.bottom()),
				ImColor(255, 255, 255, 25)
			);
			window->DrawList->AddRect(
				ImVec2(selectionRect.left(), selectionRect.top()),
				ImVec2(selectionRect.right(), selectionRect.bottom()),
				ImColor(255, 255, 255, 200)
			);
		}

		// draw anim curves
		for (AnimCurve * animCurve : curves) {
			auto & keys = animCurve->getKeys();
			for (auto i = 0; i < keys.size() - 1; i++) {
				Key keyA = keys.at(i);
				Key keyB = keys.at(i + 1);

				// draw x curve
				glm::vec2 posAx = glm::vec2(keyA.frame, keyA.value);
				glm::vec2 posBx = glm::vec2(keyB.frame, keyB.value);
				addLineSegment(posAx, posBx, animCurve->color);
			}
		}

		// modify keys
		bool anySelectedPointDragging{ false };
		for (AnimCurve * animCurve : curves) {
			auto & keys = animCurve->getKeys();
			for (auto & key : keys) {
				ImGui::PushID(&key);
				glm::vec2 C = glm::vec2(key.frame, key.value);

				ImGui::PushStyleColor(ImGuiCol_Button,
					key.selected ? ImVec4(1, 1, 1, 1) : ImVec4(1, 1, 0.5, 0.5)
				);

				if (Im2D::Button("##C", C, glm::vec2(16.0))) {
					key.selected = !key.selected;
					cout << "clicked" << " selected: " << key.selected << endl;
					cout << "       active: " << ImGui::IsItemActive() << endl;
				}

				if (key.selected && ImGui::IsItemActive() && ImGui::IsMouseDragging(0))
					anySelectedPointDragging = true;

				ImGui::PopStyleColor();

				ImGui::PopID();
			}
		}

		if (anySelectedPointDragging) {
			for (AnimCurve * animCurve : curves) {
				auto & keys = animCurve->getKeys();
				for (auto & key : keys) {
					if (key.selected) {
						key.frame += Im2D::GetMouseDelta().x;
						key.value += Im2D::GetMouseDelta().y;
					}
				}
			}
		}

		// handle shortcuts
		//for (auto i = 0; i < 512; i++)
		//	if (io.KeysDown[i])
		//		cout << i << endl;

		if (ImGui::IsWindowHovered())
			if (ImGui::IsKeyPressed(83/*s*/))
				for (auto animCurve : curves) {
					if (animCurve->hasKeyAtFrame(F)) {
						animCurve->removeKeyAtFrame(F);
					}
					else {
						animCurve->insertKeyAtFrame(F);
					}
				}

		// draw current time horizontal line
		auto top = fromScreen({ 0, ImGui::GetWindowPos().y }).y;
		auto bottom = fromScreen({ 0, ImGui::GetWindowPos().y + ImGui::GetWindowHeight() }).y;
		addLineSegment({ F, top }, { F, bottom }, ImColor(255, 255, 0, 128));

		ImVec2 mousePos = ImGui::GetMousePos();
		auto P = fromScreen(glm::vec2(mousePos.x, mousePos.y));
		ImGui::SetCursorScreenPos({ mousePos.x, mousePos.y-ImGui::GetFontSize()});
		ImGui::Text("mouse(%f, %f) -> P(%f, %f)",mousePos.x, mousePos.y ,(float)P.x, (float)P.y) ;
		

		int windowLeft = ImGui::GetWindowPos().x;
		int windowRight = ImGui::GetWindowPos().x + ImGui::GetWindowWidth();
		ImGui::SetCursorPos(ImVec2(0, 0));
		ImGui::PushItemWidth(ImGui::GetWindowWidth());
		ImGui::SliderInt("graphtime", &F, fromScreen({ windowLeft, 0 }).x, fromScreen({ windowRight, 0 }).x);
		ImGui::PopItemWidth();

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