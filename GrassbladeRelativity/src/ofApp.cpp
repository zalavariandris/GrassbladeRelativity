#include "ofApp.h"

glm::vec2 curveToRect(Curve & curve, glm::vec2 uv) {
	double distance = uv.x;
	double t = uv.y;

	glm::vec2 P = curve.getPointAtTime(t);
	glm::vec2 normal = curve.getNormal(t, true);

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

glm::vec2 rectToCurve(Curve & curve, glm::vec2 xy) {
	double t = curve.getNearestTime(xy);
	auto Q = curve.getPointAtTime(t);
	double distance = glm::distance(xy, Q);

	glm::vec2 P = curve.getPointAtTime(t);
	glm::vec2 normal = curve.getNormal(t);

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

void ofApp::setup() {
	gui.setup();
	ImGui::GetIO().FontGlobalScale = 1.5;
	camera.enableOrtho();
	camera.setNearClip(-1000);
	camera.setFarClip(1000);
	camera.setPosition(0, 0, 0);
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
	static glm::vec2 A{ 0,0 };
	static glm::vec2 B{ 7,-90 };
	static glm::vec2 C{ 50,-170 };

	glm::vec2 handleIn{ (A-C)*0.25 };
	glm::vec2 handleOut{ (C-A) *0.25 };

	Path path({
		std::make_shared<Segment>(A),
		std::make_shared <Segment>(B, handleIn, handleOut),
		std::make_shared <Segment>(C)
	});

	// Edit blade
	ImGui::SetNextWindowBgAlpha(0.0);
	ImGui::Begin("Viewport");
	Im2D::ViewerBegin("Viewer1");
	Im2D::DragPoint("A", &A);
	Im2D::DragPoint("B", &B);
	Im2D::DragPoint("C", &C);

	syncCameraToViewport(camera);
	Im2D::ViewerEnd();
	ImGui::End();

	// draw blade
	camera.begin();
	ofSetColor(ofColor::black);
	auto segments{ 8 * path.getCurves().size() };
	for (auto i = 0; i < segments; i++) {
		double t1 = (double)i / segments;
		double t2 = ((double)i + 1.0) / segments;

		glm::vec2 P1 = path.getPointAtTime(t1);
		glm::vec2 P2 = path.getPointAtTime(t2);

		ofDrawLine(P1, P2);
	};

	// draw handles
	ofDrawLine(B, B + handleIn);
	ofDrawLine(B, B + handleOut);
	camera.end();

	// extend path
	static float extension_length{ 1.0 };

	// edit grassblade extension
	ImGui::Begin("GrassbladeRelativity");
	if (ImGui::CollapsingHeader("extend grassblade", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat("extension_length", &extension_length);
	}
	ImGui::End();

	//create location
	static float time{ 0.0 };
	ImGui::Begin("GrassbladeRelativity");
	if (ImGui::CollapsingHeader("create location", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::SliderFloat("time", &time, 0.0, 1.0);
	}
	ImGui::End();
	CurveLocation loc = path.getLocationAtTime(time);

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

void ofApp::draw() {
	gui.begin();
	showPath();
	gui.end();
}

void ofApp::update() {

}