#include "ofApp.h"
#include "Paper/numerical.h"
#include "Im2DPaper/Im2DPaper.h"
#include "imgui_internal.h"
#include "Reader.h"
#include "demos.h"

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

// basic frameworks stuff
void ofApp::setup() {
	/*
	 * Setup basic stuff
	 */
	// imgui
	gui.setup();
	ImGui::GetIO().FontGlobalScale = 1.5;
	
	//OF texture handling
	ofDisableArbTex();
	//ofEnableNormalizedTexCoords(); // needless with disabled ARB textures, but left it here for clarity.
	ofSetVerticalSync(true);
	ofSetBackgroundColor(ofColor(40, 40, 40));

	// camera
	camera.enableOrtho();
	camera.setNearClip(-1000);
	camera.setFarClip(1000);
	camera.setPosition(0, 0, 0);
	
	// main 
	setupGrassblade();
}

void ofApp::update() {
	
}

// main grassbalde calls
void ofApp::setupGrassblade() {
	// paths
	sourcePath = Path({
		std::make_shared<Segment>(glm::vec2(0,100), glm::vec2(), glm::vec2(-50,-50)),
		std::make_shared<Segment>(glm::vec2(0,-100), glm::vec2(-50, 50), glm::vec2())
	});

	targetPath = Path({
		std::make_shared<Segment>(glm::vec2(-30,100), glm::vec2(), glm::vec2(-50,-50)),
		std::make_shared<Segment>(glm::vec2(-30,-100), glm::vec2(-50, 50), glm::vec2())
	});

	// geometry
	plate.set(500, 500, 10, 10);

}

void ofApp::showGrassblade() {
	ImGui::SetNextWindowBgAlpha(0.0);
	ImGui::Begin("grassblade");
	Im2D::ViewerBegin("viewport");
	for (auto segment : sourcePath.getSegments()) {
		ImGui::PushID(segment.get());
		Im2D::DragSegment("source", segment.get());
		ImGui::PopID();
	}
	for (auto segment : targetPath.getSegments()) {
		ImGui::PushID(segment.get());
		Im2D::DragSegment("target", segment.get());
		ImGui::PopID();
	}
	syncCameraToViewport(camera);
	Im2D::ViewerEnd();
	ImGui::End();

	// render with OF
	camera.begin();

	// deform mesh
	plate.set(500, 500, 10, 10); // reset mesh
	auto mesh = plate.getMeshPtr();
	for (auto i = 0; i < mesh->getVertices().size(); i++) {
		glm::vec2 P0 = mesh->getVertex(i);
		auto uv = rectToPath(sourcePath, P0);
		auto P1 = pathToRect(targetPath, uv);
		if (!isnan(P1.x) && !isnan(P1.y)) {
			mesh->setVertex(i, glm::vec3(P1, 0));
		}
	}

	ofSetColor(ofColor::cadetBlue);
	plate.draw(OF_MESH_FILL);
	ofSetColor(ofColor::white);
	plate.draw(OF_MESH_WIREFRAME);

	ofSetColor(ofColor::cyan);
	ofDraw(sourcePath);
	ofSetColor(ofColor::pink);
	ofDraw(targetPath);
	camera.end();
}


void ofApp::draw() {
	
	gui.begin();
	//showDemos();
	//showGraphEditorDemo();
	//ImGui::ShowDemoWindow();
	//showAnimationDemo();
	//showGraphEditorDemo();
	gui.end();
}