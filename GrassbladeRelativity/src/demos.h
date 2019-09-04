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

	void ofDraw(Path path, bool verbose=true) {
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
				ofDrawCircle(segment->_point, 2.0);
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
	AnimCurve(std::vector<Key> keys, std::string label="", ImColor color=ImColor(255,255,255,128))
		:label(label), color(color)
	{
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

	int getKeyIndexAtFrame(int frame) {
		for (int i = 0; i < _keys.size(); i++) {
			if (getKeys()[i].frame == frame) {
				return i;
			}
		}
		return -1;
	}

	bool hasKeyAtFrame(int frame) {
		return getKeyIndexAtFrame(frame) >= 0;
	}

	bool removeKeyAtFrame(int frame) {
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
	ImGui::SameLine();
	ImGui::Text("%.0ffps", ofGetFrameRate());
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
				if (ImRect(coords - glm::vec2(tolerance), coords + glm::vec2(tolerance)).Contains(P)) {
					return &key;
				}
			}
		}
		return nullptr;
	};

	// get all keys inside a rect
	auto getKeysInRect = [&](ImRect rect, double tolerance = 5)->std::vector<Key*> {
		std::vector<Key*> keysInRect;
		for (AnimCurve * curve : curves) {
			for (auto &key : curve->getKeys()) {
				auto P = toScreen({ key.frame, key.value });
				if (rect.Contains(P)) {
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

	ImRect selectionRect{ 0,0,0,0 };
	if (action == ACTION_SELECTING && ImGui::IsItemActive() && ImGui::IsMouseDragging(0) && !AnyModifierKey) {
		// extend rect selection
		selectionTo.x = ImGui::GetMousePos().x;
		selectionTo.y = ImGui::GetMousePos().y;

		// select keys in rectangle
		selectionRect = ImRect(selectionFrom, selectionTo);
		if (selectionRect.Min.x > selectionRect.Max.x)
			swap(selectionRect.Min.x, selectionRect.Max.x);
		if (selectionRect.Min.y > selectionRect.Max.y)
			swap(selectionRect.Min.y, selectionRect.Max.y);
		for (AnimCurve * curve : curves) {
			for (auto &key : curve->getKeys()) {
				auto P = toScreen({ key.frame, key.value });
				if (selectionRect.Contains(P)) {
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
		auto window = ImGui::GetCurrentWindow();
		window->DrawList->AddRectFilled(
			selectionRect.Min,
			selectionRect.Max,
			ImColor(255, 255, 255, 25)
		);
		window->DrawList->AddRect(
			selectionRect.Min,
			selectionRect.Max,
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
	//auto file = "C:/Users/andris/Desktop/grassfieldwind.mov";
	auto file = "C:/Users/andris/Pictures/2019-08/IMG_6926.MOV";
	static Reader reader(file);
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

namespace Field {
	glm::vec2 curveToRect(Curve const & curve, glm::vec2 uv) {
		double distance = uv.x;
		double t = uv.y;

		glm::vec2 P = curve.getPointAtTime(t);
		glm::vec2 normal = curve.getNormalAtTime(t);

		glm::vec2 xy = P + normal * distance;
		return xy;
	}

	glm::vec2 pathToRect(Path const & path, glm::vec2 uv) {
		double distance = uv.x;
		double t = uv.y;

		CurveLocation loc = path.getLocationAtTime(t);
		glm::vec2 P = loc._point;
		glm::vec2 normal = loc._normal;

		glm::vec2 xy = P + normal * distance;
		return xy;
	}

	glm::vec2 rectToCurve(Curve const & curve, glm::vec2 xy) {
		double t = curve.getNearestTime(xy);
		auto Q = curve.getPointAtTime(t);
		double distance = glm::distance(xy, Q);

		glm::vec2 P = curve.getPointAtTime(t);
		glm::vec2 normal = curve.getNormalAtTime(t);

		double dot = glm::dot(glm::normalize(xy - P), glm::normalize(normal));
		glm::vec2 uv(dot > 0 ? distance : -distance, t);
		return uv;
	}

	glm::vec2 rectToPath(Path const & path, glm::vec2 xy) {
		double t = path.getNearestTime(xy);
		auto Q = path.getPointAtTime(t);
		double distance = glm::distance(xy, Q);

		glm::vec2 P = path.getPointAtTime(t);
		glm::vec2 normal = path.getNormalAtTime(t);

		double dot = glm::dot(glm::normalize(xy - P), glm::normalize(normal));
		glm::vec2 uv(dot > 0 ? distance : -distance, t);
		return uv;
	}

	glm::vec2 pathToPath(Path const & source, Path const & target, glm::vec2 P0) {
		glm::vec2 uv = Field::rectToPath(source, P0);
		glm::vec2 P1 = Field::pathToRect(target, uv);
		return P1;
	}
}

void showGrassDemo() {
	/* TIME */
	// model
	static int F{ 0 }, begin{ 0 }, end{ 99 };
	static bool play{ false };

	// logic 
	if (play)
		F++;
	if (play && F > end)
		F = begin;

	// Gui
	ImGui::Begin("TimeSlider");
	TimeSlider("TimeSlider", &F, &play, &begin, &end);
	ImGui::End();

	/* CONTROL POINTS */
	// model
	static glm::vec2 A0{ Storage("A0") }, B0{ Storage("B0") }, C0{ Storage("C0") };
	static glm::vec2 A1{ Storage("A1") }, B1{ Storage("B1") }, C1{ Storage("C1") };
	// Gui
	ImGui::Begin("Outliner");
	ImGui::DragFloat("A1.x", &A1.x);
	ImGui::DragFloat("A1.y", &A1.y);
	ImGui::DragFloat("B1.x", &B1.x);
	ImGui::DragFloat("B1.y", &B1.y);
	ImGui::DragFloat("C1.x", &C1.x);
	ImGui::DragFloat("C1.y", &C1.y);
	ImGui::End();

	// Gizmo
	ImGui::SetNextWindowBgAlpha(0.0);
	ImGui::Begin("Grassblade");
	Im2D::ViewerBegin("viewport");
	Im2D::DragPoint("A1", &A1);
	Im2D::DragPoint("B1", &B1);
	Im2D::DragPoint("C1", &C1);
	Im2D::ViewerEnd();
	ImGui::End();

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
		make_shared<Segment>(B1, (A1 - C1)*0.25, (C1 - A1)*0.25),
		make_shared<Segment>(C1)
		});

	//extend paths
	static float length{ 250 };
	ImGui::Begin("Outliner");
	if (ImGui::CollapsingHeader("ExtendPath", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat("length", &length);
	}
	ImGui::End();

	auto firstLocation0 = path0.getLocationAtTime(0);
	auto lastLocation0 = path0.getLocationAtTime(1.0);
	path0.insert(0, make_shared<Segment>(firstLocation0._point - firstLocation0._tangent*length));
	path0.add(make_shared<Segment>(lastLocation0._point + lastLocation0._tangent*length));

	auto firstLocation1 = path1.getLocationAtTime(0);
	auto lastLocation1 = path1.getLocationAtTime(1.0);
	path1.insert(0, make_shared<Segment>(firstLocation1._point - firstLocation1._tangent*length));
	path1.add(make_shared<Segment>(lastLocation1._point + lastLocation1._tangent*length));


	/* read the movie file */
	//auto file = "C:/Users/andris/Desktop/grassfieldwind.mov";
	auto file = "C:/Users/andris/Pictures/2019-08/IMG_6926.MOV";
	static Reader reader(file);
	auto & currentImage = reader.getImageAtFrame(F);

	// draw OF
	static ofCamera cam;
	ImGui::Begin("Grassblade");
	Im2D::ViewerBegin("viewport");
	ofSyncCameraToViewport(cam);
	Im2D::ViewerEnd();
	ImGui::End();
	cam.begin();

	// deform mesh
	static ofPlanePrimitive plate;
	static float plateWidth{ 720 }, plateHeight{ 405 };
	static int plateColumns{ 10 }, plateRows{ 10 };
	ImGui::Begin("Outliner");
	if (ImGui::CollapsingHeader("CreatePlate", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat("width", &plateWidth);
		ImGui::DragFloat("height", &plateHeight);
		ImGui::DragInt("columns", &plateColumns);
		ImGui::DragInt("rows", &plateRows);
	}
	ImGui::End();

	plate.set(plateWidth, plateHeight, plateColumns, plateRows); //reset mesh
	plate.mapTexCoords(0, 1, 1, 0);
	auto mesh = plate.getMeshPtr();
	for (auto i = 0; i < mesh->getVertices().size(); i++) {
		glm::vec2 P0 = mesh->getVertex(i);
		auto P1 = Field::pathToPath(path1, path0, P0);
		if (!isnan(P1.x) && !isnan(P1.y)) {
			mesh->setVertex(i, glm::vec3(P1, 0));
		}
	}

	ofSetColor(ofColor::cadetBlue);
	ofSetColor(ofColor::white);
	currentImage.bind();
	plate.draw(OF_MESH_FILL);
	ofSetColor(ofColor(255, 40));
	currentImage.unbind();
	plate.draw(OF_MESH_WIREFRAME);
	ofSetColor(ofColor::white);
	ofDraw(path0);
	ofDraw(path1);

	cam.end();

	/* control time */
	ImGui::Begin("Outliner");
	if (ImGui::CollapsingHeader("ReadFile", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("file: %s", !reader.getFile().empty() ? reader.getFile().c_str() : "-no file-");
		ImGui::Text("dimension: %ix%ipx", reader.getWidth(), reader.getHeight());
	}
	ImGui::End();

	/* animate source path */
	static AnimCurve
		Ax{ { Key(0,70+50) } ,"Ax", ImColor(255,0,0)},
		Ay{ { Key(0,-140) } ,"Ay", ImColor(0,255,0) },
		Bx{ { Key(0,30+50)} ,"Bx", ImColor(255,0,0) },
		By{ { Key(0,-30)} ,"By", ImColor(0,255,0) },
		Cx{ { Key(0,20+50)} ,"Cx" , ImColor(255,0,0) },
		Cy{ { Key(0,125)} ,"Cy", ImColor(0,255,0) };

	ImGui::Begin("GraphEditor");
	std::vector<AnimCurve*> curves = { &Ax, &Ay, &Bx, &By, &Cx, &Cy };
	GraphEditor("GraphEditor", curves, &F);
	ImGui::End();

	// Handle shortcuts
	if (ImGui::IsKeyPressed(83/*s*/)) {
		for (AnimCurve * animCurve : curves) {
			if (animCurve->hasKeyAtFrame(F)) {
				animCurve->removeKeyAtFrame(F);
			}
			else {
				animCurve->insertKeyAtFrame(F);
			}
		}
	}

	A0 = { Ax.getValueAtFrame(F), Ay.getValueAtFrame(F) };
	B0 = { Bx.getValueAtFrame(F), By.getValueAtFrame(F) };
	C0 = { Cx.getValueAtFrame(F), Cy.getValueAtFrame(F) };

	/* layout windows*/
	//ImGui::SetNextWindowPos(ImVec2(0, 0));
	//ImGui::SetNextWindowSize(ImVec2(200, ofGetHeight()-100));
	//ImGui::Begin("Outliner"); ImGui::End();

	//ImGui::SetNextWindowPos(ImVec2(200, 0));
	//ImGui::SetNextWindowSize(ImVec2((ofGetWidth()-200) / 2, ofGetHeight() -100));
	//ImGui::Begin("Grassblade"); ImGui::End();

	//ImGui::SetNextWindowPos(ImVec2(200+(ofGetWidth() - 200) / 2, 0));
	//ImGui::SetNextWindowSize(ImVec2((ofGetWidth() - 200)/2, ofGetHeight() -100));
	//ImGui::Begin("GraphEditor"); ImGui::End();

	//ImGui::SetNextWindowPos(ImVec2(0, ofGetHeight() -100));
	//ImGui::SetNextWindowSize(ImVec2(ofGetWidth(), 100));
	//ImGui::Begin("TimeSlider"); ImGui::End();	
}

void showDemos() {
	//ImGui::ShowDemoWindow();
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