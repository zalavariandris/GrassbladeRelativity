#pragma once
#include "imgui.h"
#include "Im2D/Im2D.h"
#include "Paper/Path.h"
#include <glm/glm.hpp>
#include "Im2D/Im2D.h"
#include "Im2D/im2d_draw.h"

/* utilities */
namespace {
	void ofSyncCameraToViewport(ofCamera & camera) {
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

	void ofDraw(Path path, bool tangents=true) {
		auto segments{ 32 * path.getCurves().size() };
		for (auto i = 0; i < segments; i++) {
			double t1 = (double)i / segments;
			double t2 = ((double)i + 1.0) / segments;

			glm::vec2 P1 = path.getPointAtTime(t1);
			glm::vec2 P2 = path.getPointAtTime(t2);
			ofDrawLine(P1, P2);
		};

		if (tangents) {
			for (auto segment : path.getSegments()) {
				ofDrawLine(segment->_point, segment->_point + segment->_handleIn);
				ofDrawLine(segment->_point, segment->_point + segment->_handleOut);
			}
		}
	}

	double lerp(double a, double b, double t) {
		return a + t * (b - a);
	};

	glm::vec2 lerp(glm::vec2 A, glm::vec2 B, double t) {
		return glm::vec2(lerp(A.x, B.x, t), lerp(A.y, B.y, t));
	}
}

/* Animation */
struct Key {
	bool selected{ false };
	int frame;
	double value;
	Key(int frame, double value) :frame(frame), value(value) {};
};

class AnimCurve {
private:
	vector<Key> _keys;


public:
	ImColor color{ 255,255,255,128 };
	std::string label{ "" };
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

	double getValueAtFrame(double frame) const {
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
		for (int i = 0; i < _keys.size(); i++)
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
		for (int i = 0; i < _keys.size(); i++) {
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
		keys.erase(keys.begin() + idx);
		return true;
	}

	std::vector<Key> & getKeys() {
		return _keys;
	}
};

namespace {
	void addTrajectory(AnimCurve const & x, AnimCurve const & y, int from, int to) {
		for (auto f = from; f < to; f++) {
			glm::vec2 A = glm::vec2(x.getValueAtFrame(f), y.getValueAtFrame(f));
			glm::vec2 B = glm::vec2(x.getValueAtFrame(f + 1), y.getValueAtFrame(f + 1));
			addLineSegment(A, B, ImColor(128, 128, 128, 128));
		}
	}
}

/* Widgets */
void TimeSlider(const char * label_id, int * frame, bool * play, int * begin, int * end) {
	// time slider
	ImGui::BeginGroup();
	ImGui::PushItemWidth(50);
	ImGui::DragInt("##begin", begin);
	if (*begin >= *end)
		*begin = *end;
	ImGui::SameLine();
	ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() - 2 * (ImGui::GetWindowContentRegionWidth() - ImGui::GetContentRegionAvailWidth()));
	ImGui::SliderInt("##frame", frame, *begin, *end);
	ImGui::SameLine();
	ImGui::PushItemWidth(50);
	ImGui::DragInt("##end", end);
	if (*end <= *begin)
		*end = *begin;

	// time controls
	auto style = ImGui::GetStyle();
	float btnWidth = ImGui::GetFontSize()*1.5;
	float btnHeight = btnWidth;
	ImGui::SetCursorPosX(ImGui::GetContentRegionAvailWidth() / 2 - (btnWidth * 6 + 2 * style.ItemSpacing.x) / 2);
	ImGui::BeginGroup();
	if (ImGui::Button("|<<", ImVec2(btnWidth, btnHeight)))
		(*frame) = (*begin);
	ImGui::SameLine();
	if (ImGui::Button("|<", ImVec2(btnWidth, btnHeight)))
		(*frame)--;
	ImGui::SameLine();
	if (ImGui::Button("||", ImVec2(btnWidth, btnHeight)))
		*play = false;
	ImGui::SameLine();
	if (ImGui::Button(">", ImVec2(btnWidth, btnHeight)))
		*play = true;
	ImGui::SameLine();
	if (ImGui::Button(">|", ImVec2(btnWidth, btnHeight)))
		(*frame)++;
	ImGui::SameLine();
	if (ImGui::Button(">>|", ImVec2(btnWidth, btnHeight)))
		*frame = *end;
	ImGui::EndGroup();//end buttons group
	ImGui::EndGroup();//end timeslider group
}

void GraphEditor(const char * label_id, std::vector<AnimCurve*> curves, int * F) {
	// start viewer
	Im2D::ViewerBegin(label_id, ImVec2(), Im2DViewportFlags_Grid | Im2DViewportFlags_AllowNonUniformZoom);

	/*
	 * Handle select and move
	 */
	enum Action {
		ACTION_IDLE,
		ACTION_SELECTING,
		ACTION_MOVING
	};

	// get a single key at specified coordinates
	auto getKeyAtCoords = [&](glm::vec2 coords, double tolerance = 5)->Key* {
		for (AnimCurve * curve : curves) {
			for (auto &key : curve->getKeys()) {
				auto P = toScreen({ key.frame, key.value });
				if (Rect(coords - glm::vec2(tolerance), coords + glm::vec2(tolerance)).contains(P)) {
					return &key;
				}
			}
		}
		return nullptr;
	};

	// get all keys inside a rect
	auto getKeysInRect = [&](Rect rect, double tolerance = 5)->std::vector<Key*> {
		std::vector<Key*> keysInRect;
		for (AnimCurve * curve : curves) {
			for (auto &key : curve->getKeys()) {
				auto P = toScreen({ key.frame, key.value });
				if (rect.contains(P)) {
					keysInRect.push_back(&key);
				}
			}
		}
		return keysInRect;
	};

	static Action action = ACTION_IDLE;
	static glm::vec2 selectionFrom;
	static glm::vec2 selectionTo;
	bool AnyModifierKey = ImGui::GetIO().KeyCtrl || ImGui::GetIO().KeyAlt || ImGui::GetIO().KeyShift;
	if (action == ACTION_IDLE && ImGui::IsItemClicked(0) && !AnyModifierKey) {
		// get keys under mouse
		auto mouseScreenPos = glm::vec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
		Key * keyHit = getKeyAtCoords(mouseScreenPos);
		bool anyKeyHit = keyHit == nullptr ? false : true;

		//
		if (anyKeyHit) {
			if (keyHit->selected) {
				// start moving selected keys
				action = ACTION_MOVING;
			}
			else {
				//deselect all keys
				for (AnimCurve * curve : curves)
					for (auto & key : curve->getKeys())
						key.selected = false;

				keyHit->selected = true;
				action = ACTION_MOVING;
			}

		}
		else {
			//deselect all keys
			for (AnimCurve * curve : curves)
				for (auto & key : curve->getKeys())
					key.selected = false;

			// start rect selection
			action = ACTION_SELECTING;
			selectionFrom.x = ImGui::GetMousePos().x;
			selectionFrom.y = ImGui::GetMousePos().y;
			selectionTo.x = selectionFrom.x;
			selectionTo.y = selectionFrom.y;
		}
	}

	if (action == ACTION_SELECTING && ImGui::IsItemActive() && ImGui::IsMouseDragging(0) && !AnyModifierKey) {
		// extend rect selection
		selectionTo.x = ImGui::GetMousePos().x;
		selectionTo.y = ImGui::GetMousePos().y;

		// select keys in rectangle
		Rect selectionRect(selectionFrom, selectionTo);
		for (AnimCurve * curve : curves) {
			for (auto &key : curve->getKeys()) {
				auto P = toScreen({ key.frame, key.value });
				if (selectionRect.contains(P)) {
					key.selected = true;
				}
				else {
					key.selected = false;
				}
			}
		}
	}

	if (action == ACTION_MOVING && ImGui::IsItemActive() && ImGui::IsMouseDragging(0) && !AnyModifierKey) {
		// move keys with drag
		glm::vec2 delta = Im2D::GetMouseDelta();
		for (AnimCurve * curve : curves) {
			for (auto & key : curve->getKeys()) {
				if (key.selected) {
					key.frame += delta.x;
					key.value += delta.y;
				}
			}
		}
	}
	if (ImGui::IsItemDeactivated()) {
		action = ACTION_IDLE;
	}

	// draw animation curves
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

	// draw animation keys
	for (AnimCurve * animCurve : curves) {
		auto keys = animCurve->getKeys();
		for (auto key : keys) {
			addRect(glm::vec2(key.frame, key.value), 5, 5, key.selected ? ImColor(255, 255, 255) : ImColor(128, 128, 128), 2.0);
		}
	}

	// draw animCurve labels
	auto window = ImGui::GetCurrentWindow();
	for (AnimCurve * animCurve : curves) {
		float screenLeft = ImGui::GetWindowPos().x + ImGui::GetStyle().ItemSpacing.x;
		float viewLeft = fromScreen({ screenLeft,0 }).x;
		ImVec2 labelPos;
		auto firstKey = animCurve->getKeys().at(0);
		if (viewLeft > firstKey.frame) {
			float valueAtLeft = animCurve->getValueAtFrame(viewLeft);
			float screenValue = toScreen({ 0, valueAtLeft }).y;
			labelPos = ImVec2(
				screenLeft,
				screenValue
			);
		}
		else {
			auto screenPos = toScreen({ firstKey.frame, firstKey.value });
			labelPos = ImVec2(screenPos.x, screenPos.y);
		}
		labelPos.y -= ImGui::GetFontSize();

		window->DrawList->AddText(labelPos, animCurve->color, animCurve->label.c_str());
	}

	// draw current time
	auto top = fromScreen({ 0, ImGui::GetWindowPos().y }).y;
	auto bottom = fromScreen({ 0, ImGui::GetWindowPos().y + ImGui::GetWindowHeight() }).y;
	addLineSegment({ *F, top }, { *F, bottom }, ImColor(255, 255, 0, 128));

	//draw selection rectangle
	if (action == ACTION_SELECTING) {
		Rect selectionRect(selectionFrom, selectionTo);
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

	Im2D::ViewerEnd();
}

/* Demos */
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

void showReadDemo() {
	auto file = "C:/Users/andris/Desktop/grassfieldwind.mov";/*test file: ofToDataPath("framecounter.mov")*/
	static Reader reader(file, false);
	ImGui::Text("file: %s", !reader.getFile().empty() ? reader.getFile().c_str() : "-no file-");
	ImGui::Text("dimension: %ix%ipx", reader.getWidth(), reader.getHeight());
	ImGui::Separator();

	int start{ 0 };
	int end{ reader.getFrameCount()-1 };
	static int F{ 0 };
	static bool play{ false };
	TimeSlider("timeslider", &F, &play, &start, &end);
	
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

void showGraphEditorDemo() {
	static AnimCurve animCurveX({
		Key(0,0),
		Key(50,50),
		Key(100,100),
		Key(150,150),
		Key(200,200),
		Key(250,250)
	});
	animCurveX.color = ImColor(255, 0, 0);
	animCurveX.label = "x";

	static AnimCurve animCurveY({
		Key(0,250),
		Key(50,200),
		Key(100,50),
		Key(150,100),
		Key(200,50),
		Key(250,50)
		});
	animCurveY.color = ImColor(0, 255, 0);
	animCurveY.label = "y";
	static int F{ 0 };
	
	ImGui::Begin("GraphEditor");
	ImGui::DragInt("F", &F);
	GraphEditor("grapheditor", { &animCurveX, &animCurveY }, &F);
	ImGui::End();
}

void showTimeSliderDemo() {
	ImGui::Begin("TimeSliderDemo");
	static int F, begin, end{100};
	static bool play;
	if (play)
		F++;
	ImGui::DragInt("F", &F);
	ImGui::DragInt("begin", &begin);
	ImGui::DragInt("end", &end);
	ImGui::Checkbox("play", &play);
	ImGui::Separator();
	TimeSlider("timeslider", &F, &play, &begin, &end);
	ImGui::End();
}

void showAnimationDemo() {
	/* Model */
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
	animCurveX.label = "X";

	static AnimCurve animCurveY({
		Key(0,250),
		Key(50,200),
		Key(100,50),
		Key(150,100),
		Key(200,50),
		Key(250,50)
		});
	animCurveY.color = ImColor(0, 255, 0);
	animCurveY.label = "Y";

	/* Logic */
	if (play) {
		F++;
		if (F > end)
			F = begin;
		if (F < begin)
			F = end;
	}

	// set position of point from animatin curves
	P = {
		animCurveX.getValueAtFrame(F),
		animCurveY.getValueAtFrame(F)
	};

	/* GUI */

	/* Canvas Viewer */
	ImGui::Begin("Viewer");
	Im2D::ViewerBegin("viewer");
	// set animCurves when dragging the point
	if (Im2D::DragPoint("P", &P)) {
		animCurveX.setValueAtFrame(P.x, F);
		animCurveY.setValueAtFrame(P.y, F);
	}

	// draw trajectory
	static int framesBefore{ 100 };
	static int framesAfter{ 100 };
	ImGui::DragInt("onionBefore", &framesBefore);
	ImGui::DragInt("onionAfter", &framesAfter);
	addTrajectory(animCurveX, animCurveY, F-framesBefore, F+framesAfter);

	//add plot
	

	Im2D::ViewerEnd();
	ImGui::End();

	/* control time with a __TimeSlider__ */
	ImGui::Begin("TimeSlider");
	TimeSlider("TimeSlider", &F, &play, &begin, &end);
	ImGui::End();

	/* Show animCurves in the __GraphEditor__ */
	ImGui::Begin("GraphEditor");
	std::vector<AnimCurve*> curves{ &animCurveX, &animCurveY };

	// Toolbar
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

	// Editor space
	GraphEditor("GraphEditor", {&animCurveX, &animCurveY}, &F);

	// Handle shortcuts
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
	ImGui::End();
}

glm::vec2 Storage(std::string id) {
	static std::map<std::string, glm::vec2> storage{
		{"A0", {20, -140}},
		{"B0", {-20, -30}},
		{"C0", {-30, 125}},
		{"A1", {70, -140}},
		{"B1", {30, -30}},
		{"C1", {20, 125}},
	};
	return storage[id];
}

void showGrassDemo() {
	ImGui::SetNextWindowBgAlpha(0.0);
	ImGui::Begin("Grassblade"); // TODO: reenter Im2D viewport
	Im2D::ViewerBegin("viewport");

	/* create control points */
	static glm::vec2 A0{ Storage("A0") }, B0{ Storage("B0") }, C0{ Storage("C0") };
	static glm::vec2 A1{ Storage("A1") }, B1{ Storage("B1") }, C1{ Storage("C1") };

	Im2D::DragPoint("A0", &A0);
	Im2D::DragPoint("B0", &B0);
	Im2D::DragPoint("C0", &C0);
	Im2D::DragPoint("A1", &A1);
	Im2D::DragPoint("B1", &B1);
	Im2D::DragPoint("C1", &C1);

	/* create the source and the target paths from points */
	Path path0;
	path0.add({ 
		make_shared<Segment>(A0),
		make_shared<Segment>(B0, (A0 - C0)*0.25, (C0 - A0)*0.25),
		make_shared<Segment>(C0)
	});
	Path path1;
	path1.add({ 
		make_shared<Segment>(A1),
		make_shared<Segment>(B1, (A1-C1)*0.25, (C1-A1)*0.25), 
		make_shared<Segment>(C1)
	});
	
	static ofCamera cam;
	ofSyncCameraToViewport(cam);
	cam.begin();
	ofDraw(path0);
	ofDraw(path1);
	cam.end();

	/* read the movie file */
	static std::string file;

	/* control time */
	static int F{ 0 }, begin{ 0 }, end{ 100 };

	/* animate source path */
	//AnimCurve Ax, Ay, Bx, By, Cx, Cy;
	//A0 = { Ax.getValueAtFrame(F), Ay.getValueAtFrame(F) };
	//B0 = { Bx.getValueAtFrame(F), By.getValueAtFrame(F) };
	//C0 = { Cx.getValueAtFrame(F), Cy.getValueAtFrame(F) };

	Im2D::ViewerEnd();
	ImGui::End();
}

void showDemos() {
	ImGui::ShowDemoWindow();
	// show model demos
	//if (ImGui::Begin("ReadDemo")) {
	//	showReadDemo();
	//}ImGui::End();

	//ImGui::SetNextWindowBgAlpha(0.0);
	//if (ImGui::Begin("CurveDemo")) {
	//	showCurveDemo();
	//}ImGui::End();

	//ImGui::SetNextWindowBgAlpha(0.0);
	//if (ImGui::Begin("PathDemo")) {
	//	showPathDemo();
	//}ImGui::End();

	//show widgets demos
	//showTimeSliderDemo();
	//showGraphEditorDemo();
	//showAnimationDemo();
	showGrassDemo();
}