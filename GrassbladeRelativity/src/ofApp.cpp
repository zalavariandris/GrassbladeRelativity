#include "ofApp.h"
#include "Paper/numerical.h"
#include "Im2DPaper/Im2DPaper.h"
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include "imgui_internal.h"

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

namespace Paper {
	void draw(Segment segment) {
		ofSetColor(ofColor::grey);
		ofDrawCircle(segment._point, 10);
		ofDrawCircle(segment._point + segment._handleIn, 5);
		ofDrawCircle(segment._point + segment._handleOut, 5);
		ofSetColor(ofColor(128, 128, 128, 128));
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

	void draw(Path path) {
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

	Im2D::DragPoint("P", &P);
	static Path path = Path({
		std::make_shared<Segment>(A, glm::vec2(), B - A),
		std::make_shared<Segment>(D, C - D, E-D),
		std::make_shared<Segment>(G, F-G, glm::vec2())
	});

	Im2D::DragSegment("S0", path.getSegments()[0].get(), "Smooth");
	Im2D::DragSegment("S1", path.getSegments()[1].get(), "Smooth");
	Im2D::DragSegment("S2", path.getSegments()[2].get(), "Smooth");

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

class Reader {
	cv::VideoCapture _cap;
	std::string _file;
	std::vector<ofImage> _cache;
	bool _useCache{ true };
public:
	Reader(std::string file, bool useCache){
		if (_cap.open(file)) {
			_file = file;
		}
		_useCache = useCache;
		_cache = std::vector<ofImage>(getFrameCount());
	}

	bool validateFrame(int frame) const{
		return frame >= 0 && frame < getFrameCount();
	}

	void enableCache() {
		_useCache = true;
	}

	void disableCache() {
		_useCache = false;
	}

	cv::Mat getMatAtFrame(int frame) {
		cv::Mat mat;
		if (validateFrame(frame)) {
			_cap.set(cv::CAP_PROP_POS_FRAMES, frame);
			_cap >> mat;
		}
		return mat;
	}

	int getFrameCount() const{
		return _cap.get(cv::CAP_PROP_FRAME_COUNT);
	}

	int getWidth() {
		return _cap.get(cv::CAP_PROP_FRAME_WIDTH);
	}

	int getHeight() {
		return _cap.get(cv::CAP_PROP_FRAME_HEIGHT);
	}

	int getCurrentFrame() {
		return _cap.get(cv::CAP_PROP_POS_FRAMES);
	}

	std::string getFile() {
		return _file;
	}

	~Reader() {
		_cap.release();
	}

	bool hasCacheAtFrame(int frame) const {
		return validateFrame(frame) ? _cache[frame].isAllocated() : false;
	}

	ofImage getImageAtFrame(int frame) {
		if (!validateFrame(frame))
			return ofImage();
		if (_useCache && hasCacheAtFrame(frame))
			return _cache[frame];

		auto mat = getMatAtFrame(frame);
		ofImage img;
		img.setFromPixels(mat.data, mat.cols, mat.rows, OF_IMAGE_COLOR, false);
		if (_useCache)
			_cache[frame] = img;
		return img;
	}

	ofImage play() {
		cv::Mat mat;
		ofImage img;
		if (_cap.read(mat)) {
			
			img.setFromPixels(mat.data, mat.cols, mat.rows, OF_IMAGE_COLOR, false);
		}
		return img;
	}
};

namespace ImGui {
	bool Timeslider(char * label_id, int * frame, int v_min, int v_max, Reader const & reader) {
		bool changed{ false };

		ImVec2 screenPos = ImGui::GetCursorScreenPos();
		
		if (ImGui::SliderInt(label_id, frame, v_min, v_max)) {
			changed = true;
		}
		auto window = ImGui::GetCurrentWindow();
		int left;
		int right;
		auto isCached = [&](int f) {
			return reader.hasCacheAtFrame(f);
		};
		auto singleWidth = ImGui::CalcItemWidth() / (v_max - v_min);
		for (auto i = v_min; i < v_max; i++) {
			if (isCached(i)) {
				window->DrawList->AddRect(
					ImVec2(screenPos.x+i*singleWidth, screenPos.y), 
					ImVec2(screenPos.x+(i + 1)*singleWidth, screenPos.y+10), 
					ImColor(0, 255, 0)
				);
			}
		}
		return changed;
	}
}

void showCVReadDemo() {
	static Reader reader("C:/Users/andris/Pictures/2019-08/IMG_6926.MOV", false);

	static int myFrameNumber{ 0 };


	ImGui::SliderInt("frame", &myFrameNumber, 0, reader.getFrameCount()-1);
	ImGui::Text("%f.0 fps", ofGetFrameRate());
	ImGui::Text("%s", reader.getFile().c_str());
	ImGui::Text("current frame: %i", reader.getCurrentFrame());
	ImGui::Timeslider("timeslider", &myFrameNumber, 0, reader.getFrameCount(), reader);

	if (reader.hasCacheAtFrame(myFrameNumber)) {
		ofSetColor(ofColor::green);
		ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
	}
	auto border = 2;
	//ofImage img = reader.getImageAtFrame(myFrameNumber);
	ofImage img = reader.play();

	float MB = 0.0;
	float MPPerFrame = 3*img.getWidth()*img.getHeight() / 1000000.0;
	for (int i = 0; i < reader.getFrameCount(); i++) {
		if (reader.hasCacheAtFrame(i))
			MB+=MPPerFrame;
	}
	ImGui::Text("%.0fmb", MB);
	ofSetColor(ofColor::white);
	img.draw(border, border, ofGetWidth()-border*2, ofGetHeight()-border*2);	
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
	Paper::draw(sourcePath);
	ofSetColor(ofColor::pink);
	Paper::draw(targetPath);
	camera.end();
}

void ofApp::draw() {
	
	gui.begin();
	//showPathDemo();
	//showReaderDemo();
	showCVReadDemo();
	//showGrassblade();
	gui.end();
}

