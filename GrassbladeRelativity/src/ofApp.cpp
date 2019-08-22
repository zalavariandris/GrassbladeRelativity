#include "ofApp.h"
#include "Paper/numerical.h"

glm::vec2 curveToRect(Curve & curve, glm::vec2 uv) {
	double distance = uv.x;
	double t = uv.y;

	glm::vec2 P = curve.getPointAtTime(t);
	glm::vec2 normal = curve.getNormalAtTime(t);

	glm::vec2 xy = P + normal * distance;
	return xy;
}

glm::vec2 pathToRect(Path & path, glm::vec2 uv) {
	double distance = uv.x;
	double t = uv.y;

	CurveLocation loc = path.getLocationAtTime(t);
	glm::vec2 P = loc._point;
	glm::vec2 normal = loc._normal;

	glm::vec2 xy = P + normal * distance;
	return xy;
}

glm::vec2 field(glm::vec2 pos, std::function<glm::vec2(glm::vec2)> f) {
	return f(pos);
}

glm::vec2 rectToCurve(Curve & curve, glm::vec2 xy) {
	double t = curve.getNearestTime(xy);
	auto Q = curve.getPointAtTime(t);
	double distance = glm::distance(xy, Q);

	glm::vec2 P = curve.getPointAtTime(t);
	glm::vec2 normal = curve.getNormalAtTime(t);

	double dot = glm::dot(glm::normalize(xy - P), glm::normalize(normal));
	glm::vec2 uv(dot > 0 ? distance : -distance, t);
	return uv;
}

glm::vec2 rectToPath(Path & path, glm::vec2 xy) {
	double t = path.getNearestTime(xy);
	auto Q = path.getPointAtTime(t);
	double distance = glm::distance(xy, Q);

	glm::vec2 P = path.getPointAtTime(t);
	glm::vec2 normal = path.getNormalAtTime(t);

	double dot = glm::dot(glm::normalize(xy - P), glm::normalize(normal));
	glm::vec2 uv(dot > 0 ? distance : -distance, t);
	return uv;
}

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

bool DragSegment(Segment * segment, std::string type = "BezierCorner") {
	bool changed{ false };
	// get points
	glm::vec2 handleIn = segment->_point + segment->_handleIn;
	glm::vec2 handleOut = segment->_point + segment->_handleOut;
	glm::vec2 P = segment->_point;

	// drag points
	if (Im2D::DragPoint("P", &P)) {
		segment->_point = P;
		changed = true;
	}
	if (Im2D::DragPoint("in", &handleIn)) {
		segment->_handleIn = handleIn - P;
		changed = true;
	}
	if (Im2D::DragPoint("out", &handleOut)) {
		segment->_handleOut = handleOut - P;
		changed = true;
	}
	
	return changed;
}

namespace Paper {
	void draw(Path path) {
		ofSetColor(ofColor::white);
		auto segments{ 8 * path.getCurves().size() };
		for (auto i = 0; i < segments; i++) {
			double t1 = (double)i / segments;
			double t2 = ((double)i + 1.0) / segments;

			glm::vec2 P1 = path.getPointAtTime(t1);
			glm::vec2 P2 = path.getPointAtTime(t2);
			ofDrawLine(P1, P2);
		};
		for (auto segment : path._segments) {
			ofDrawLine(segment->_point, segment->_point + segment->_handleIn);
			ofDrawLine(segment->_point, segment->_point + segment->_handleOut);
		}
	}

	void draw(Segment segment) {
		ofSetColor(ofColor::grey);
		ofDrawCircle(segment._point, 10);
		ofDrawCircle(segment._point + segment._handleIn, 5);
		ofDrawCircle(segment._point + segment._handleOut, 5);
		ofDrawLine(segment._point, segment._point + segment._handleIn);
		ofDrawLine(segment._point, segment._point + segment._handleOut);
	}

	void draw(Curve curve) {
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
	ImGui::Begin("CurveDemo");
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
	ImGui::End();

	cam.begin();
	ofSetColor(ofColor::black);
	Paper::draw(*curve);
	Paper::draw(*(curve->_segment1));
	Paper::draw(*(curve->_segment2));
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

	static glm::vec2 A{   0,  200 };
	static glm::vec2 B{ -60,  140 };
	static glm::vec2 C{-100,   60 };
	static glm::vec2 D{-100,    0 };
	static glm::vec2 E{-100, - 60 };
	static glm::vec2 F{ -60, -140 };
	static glm::vec2 G{   0, -200 };

	static glm::vec2 P{ 30, 60 };

	ImGui::SetNextWindowBgAlpha(0.0);
	ImGui::Begin("PathDemo");
	Im2D::ViewerBegin("viewport");
	//Im2D::DragPoint("A", &A);
	//Im2D::DragPoint("B", &B);
	//Im2D::DragPoint("C", &C);
	//Im2D::DragPoint("D", &D);
	//Im2D::DragPoint("E", &E);
	//Im2D::DragPoint("F", &F);
	//Im2D::DragPoint("G", &G);

	Im2D::DragPoint("P", &P);

	static Path path = Path({
		std::make_shared<Segment>(A, glm::vec2(), B - A),
		std::make_shared<Segment>(D, C - D, E-D),
		std::make_shared<Segment>(G, F-G, glm::vec2())
	});

	DragSegment(path.getFirstSegment().get());

	double t = path.getNearestTime(P);
	ImGui::Text("time: %f", (float)t);
	glm::vec2 Q = path.getPointAtTime(t);

	syncCameraToViewport(cam);
	Im2D::ViewerEnd();
	ImGui::End();

	cam.begin();
	ofSetColor(ofColor::black);
	Paper::draw(path);
	ofSetColor(ofColor::white);
	ofDrawCircle(Q, 10);
	ofDrawLine(P, Q);
	cam.end();
}

void ofApp::showPath() {
	//
	ImGui::Begin("GrassbladeRelativity");
	if (ImGui::ListBoxHeader("List", 2))
	{
		ImGui::Selectable("Selected", true);
		ImGui::Selectable("Not Selected", false);
		ImGui::ListBoxFooter();
	}
	ImGui::End();

	// Create blade
	static glm::vec2 A{  -10,    0 };
	static glm::vec2 B{ -2, -190 };
	static glm::vec2 C{ 40, -360 };

	static glm::vec2 handleIn{ B+(A-C) * 0.25 };
	static glm::vec2 handleOut{ B+(C-A) *0.25 };

	static bool KeepHandlesSmooth{ true };
	if (KeepHandlesSmooth) {
		handleIn = B + (A - C) * 0.25;
		handleOut = B + (C - A) *0.25;
	}

	ImGui::Begin("GrassbladeRelativity");
	if (ImGui::CollapsingHeader("create blade", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Checkbox("Keep handles smooth", &KeepHandlesSmooth);
	}
	ImGui::End();

	targetPath = Path({
		std::make_shared<Segment>(glm::vec2( -10, 0 )),
		std::make_shared<Segment>(glm::vec2( -2, -190), glm::vec2(0, 60), glm::vec2(0, -60)),
		std::make_shared<Segment>(glm::vec2(40, -360))
	});

	sourcePath = Path({
		std::make_shared<Segment>(A),
		std::make_shared<Segment>(B, handleIn - B, handleOut - B),
		std::make_shared<Segment>(C)
	});

	// Edit blade in viewport
	ImGui::SetNextWindowBgAlpha(0.0);
	ImGui::Begin("Viewport");
	Im2D::ViewerBegin("Viewer1");
	Im2D::DragPoint("A", &A);
	Im2D::DragPoint("B", &B);
	Im2D::DragPoint("C", &C);
	if (!KeepHandlesSmooth) {
		Im2D::DragPoint("in", &handleIn);
		Im2D::DragPoint("out", &handleOut);
	}
	syncCameraToViewport(camera);
	Im2D::ViewerEnd();
	ImGui::End();

	// extend path
	static float extension_length{ 500.0 };
	ImGui::Begin("GrassbladeRelativity");
	if (ImGui::CollapsingHeader("extend grassblade", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat("extension_length", &extension_length);
	}
	ImGui::End();
	glm::vec2 tangentBegin = sourcePath.getTangentAtTime(0);
	if (tangentBegin == glm::vec2()) {
		double tMin = Numerical::CURVETIME_EPSILON;
		auto firstPoint = sourcePath.getFirstSegment()->_point;
		auto secondPoint = sourcePath.getFirstCurve()->getPointAtTime(tMin);
		tangentBegin = secondPoint - firstPoint;
		tangentBegin = glm::normalize(tangentBegin);
	}
	auto inSegment = std::make_shared<Segment>(sourcePath.getFirstSegment()->_point - tangentBegin * extension_length);
	sourcePath.insert(0, inSegment);

	glm::vec2 tangentEnd = sourcePath.getTangentAtTime(1.0);
	
	if (tangentEnd == glm::vec2() ) {
		double tMax = 1.0-Numerical::CURVETIME_EPSILON; // TODO: 1-CURVETIME_EPSILON produces the same point as the last one insted one before, check glm precision and compare epsilon values from js to c++
		auto lastButOnePoint = sourcePath.getLastCurve()->getPointAtTime(tMax);
		auto lastPoint = sourcePath.getLastSegment()->_point;
		cout << "lastButOnePoint: " << lastButOnePoint << endl;
		cout << "lastPoint: " << lastPoint << endl;
		tangentEnd = lastPoint - lastButOnePoint;
		tangentEnd = glm::normalize(tangentEnd);
		cout << "tangentEnd: " << tangentEnd << endl;
	}
	
	auto outSegment = std::make_shared<Segment>(sourcePath.getLastSegment()->_point + tangentEnd * extension_length);
	sourcePath.add(outSegment);

	// draw blade
	camera.begin();
	Paper::draw(sourcePath);
	Paper::draw(targetPath);
	camera.end();

	//create location
	static float time{ 0.5 };
	ImGui::Begin("GrassbladeRelativity");
	if (ImGui::CollapsingHeader("create location", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::SliderFloat("time", &time, 0.0, 1.0);
	}
	ImGui::End();
	CurveLocation loc = sourcePath.getLocationAtTime(time);

	// draw Location
	camera.begin();
	ofSetColor(ofColor::blue);
	ofDrawCircle(loc._point, 5);
	ofSetColor(ofColor::darkGreen);
	ofDrawLine(loc._point, loc._point + loc._tangent * 20);
	ofSetColor(ofColor::red);
	ofDrawLine(loc._point, loc._point + loc._normal * 20);
	camera.end();
}

void ofApp::setup() {
	// setup gui
	gui.setup();
	ImGui::GetIO().FontGlobalScale = 1.5;

	//setup camera
	camera.enableOrtho();
	camera.setNearClip(-1000);
	camera.setFarClip(1000);
	camera.setPosition(0, 0, 0);

	// setup movie
	SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
	movie.load("C:/Users/andris/Pictures/2019-08/C0004.MP4");
	movie.play();
	movie.setPaused(true);
	movie.setFrame(1);
	movie.update();

	ofDisableArbTex();
	//ofEnableNormalizedTexCoords(); // needless with disabled ARB textures, but left it here for clarity.
	ofSetVerticalSync(true);
	ofSetBackgroundColor(ofColor(40, 40, 40));
	
	plane.set(1920, 1920 * movie.getHeight() / movie.getWidth());
	plane.setResolution(3, 3);
	plane.mapTexCoords(0, movie.getHeight(), movie.getWidth(), 0);
}

void ofApp::update() {
	movie.update();
}

void ofApp::draw() {
	
	gui.begin();
	showPathDemo();
	gui.end();
	//showPathDemo();
	//showPath();

	//// deform mesh
	//auto mesh = plane.getMesh();
	//for (auto i = 0; i < mesh.getVertices().size(); i++) {
	//	glm::vec2 P0 = plane.getMesh().getVertex(i);
	//	auto uv = rectToPath(sourcePath, P0);
	//	auto P1 = pathToRect(targetPath, uv);
	//	if (!isnan(P1.x) && !isnan(P1.y)) {
	//		mesh.setVertex(i, glm::vec3(P1, 0));
	//	}
	//}
	// draw emsh
	//camera.begin();
	//texture = movie.getTexture();
	//ofSetColor(ofColor(255, 255, 255, 128));
	//texture.bind();
	//plane.draw(OF_MESH_WIREFRAME);
	//texture.unbind();
	//camera.end();
}

