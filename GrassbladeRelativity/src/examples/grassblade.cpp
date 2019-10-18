
#include "../Widgets/Timeslider.hpp"
#include "../Widgets/Grapheditor.hpp"
#include "../Im2D/Im2D.h"
#include "../Im2D/im2d_draw.h"
#include "../Im2DPaper/Im2DPaper.h"
#include "../Paper/Path.h"
#include "../utilities.h"
#include "glm/ext.hpp"


#include "ofAppRunner.h"
#include "ofMain.h"
#include <json.hpp>

#include "grassblade.h"

#include "../Animation/Translators/AEAnimationData.h"
#include "../Animation/Translators/MayaAnim.h"
#include "grassblade_utilities.h"

#include <future>
#include <chrono>
#include <opencv2/opencv.hpp>

#include <map>
#include "../utilities.h"

cv::Mat renderSingle(
	int frame, int w, int h,
	Reader * reader, Paper::Path sourcePath, Animation::AnimationCurve Ax, Animation::AnimationCurve Ay, Paper::Path targetPath,
	double * progress = nullptr)
{
	// get video image
	cv::Mat videoMat;
	resize(reader->getMatAtFrame(frame), videoMat, cv::Size(w, h));

	// animate source path
	sourcePath.getSegments()[0].point( glm::vec2({ Ax.getValueAtFrame(frame), Ay.getValueAtFrame(frame) }) );

	// extend paths
	auto extendedSourcePath = extend(sourcePath, 950);
	auto extendedTargetPath = extend(targetPath, 950);

	// deform image
	cv::Mat map_x(videoMat.size(), CV_32FC1);
	cv::Mat map_y(videoMat.size(), CV_32FC1);
	Rect matRect({ 0,0 }, w, h);
	Rect camRect({ -1920 / 2, -1080 / 2 }, 1920, 1080);
	for (auto y = 0; y < videoMat.rows; y++)
	{
		for (auto x = 0; x < videoMat.cols; x++) {
			if (progress) {
				*progress = (double)y / videoMat.rows;
			}
			glm::vec2 P = Rect::map(glm::vec2(x, y), matRect, camRect);
			
			Geo::Polygon sourcePoly(divide(extendedSourcePath, 10));
			Geo::Polygon targetPoly(divide(extendedTargetPath, 10));
			glm::vec2 pos = Field::pathToPath(extendedSourcePath, extendedTargetPath, P);
			pos = Rect::map(pos, camRect, matRect);
			map_x.at<float>(y, x) = pos.x;
			map_y.at<float>(y, x) = pos.y;
		}
	}
	cv::Mat resultMat(videoMat.size(), videoMat.type());
	cv::remap(videoMat, resultMat, map_x, map_y, cv::INTER_LINEAR);

	return resultMat;
};

void renderSequence(
	int start, int end, int w, int h,
	Reader * reader, Paper::Path sourcePath, Paper::Path targetPath, Animation::AnimationCurve Ax, Animation::AnimationCurve Ay,
	double * progress = nullptr
) {
	for (auto frame = start; frame < end; frame++) {
		if (progress) {
			*progress = (double)(frame - start) / (end - start);
		}
		cv::Mat image = renderSingle(frame, w, h, reader, sourcePath, Ax, Ay, targetPath, progress);
		stringstream ss;
		ss << "tmp/" << "IMG_6923_selfrelativity_" << std::setfill('0') << std::setw(5) << frame << ".png";
		cv::imwrite(ss.str(), image);
	}
};

Grassblade::Grassblade() {
	// create plate
	static float plateWidth{ 1920 }, plateHeight{ 1080 };
	static int plateColumns{ 100 }, plateRows{ 100 };
	plate.set(plateWidth, plateHeight, plateColumns, plateRows);
	plate.mapTexCoords(0, 1, 1, 0);
	//for (auto & vertex : plate.getMesh().getVertices()) {
	//	vertex.x += plateWidth / 2;
	//	vertex.y += plateHeight / 2;
	//}

	plateMesh = ofMesh(plate.getMesh());

	distortionTexture.allocate(1920, 1080, GL_RGBA32F);
	renderFbo.allocate(1920, 1080, GL_RGBA32F);

	// paths
	sourcePath = Paper::Path({
		Paper::Segment(glm::vec2(0,0)),
		Paper::Segment(glm::vec2(0,100), glm::vec2(0,0), glm::vec2(0,0))
	});

	targetPath = Paper::Path({
		Paper::Segment(glm::vec2(0,0)),
		Paper::Segment(glm::vec2(0,100), glm::vec2(0,0), glm::vec2(0,0))
	});

	//
	open();
	reader = Reader(moviefile);
};

void Grassblade::save() {
	json j;
	j["startTime"] = startTime;
	j["endTime"] = endTime;
	j["movie"]["filename"] = moviefile;
	j["deformFactor"] = deformFactor;
	j["extensionLength"] = extensionLength;
	j["animation"]["Ax"]["keys"] = Ax.keys();
	j["sourcePath"]["segment1"]["point"]["x"] = sourcePath.getSegments()[1].point().x;
	j["sourcePath"]["segment1"]["point"]["y"] = sourcePath.getSegments()[1].point().y;
	j["sourcePath"]["segment1"]["handleIn"]["x"] = sourcePath.getSegments()[1].handleIn().x;
	j["sourcePath"]["segment1"]["handleIn"]["y"] = sourcePath.getSegments()[1].handleIn().y;

	std::ofstream o(filepath);
	o << std::setw(2) << j.dump() << std::endl;
	o.close();
	cout << "save:" << endl << j.dump() << endl;
}

void Grassblade::open() {
	ofFile file;
	if (!file.open(filepath)) cout << "cannot open file" << endl;

	json j;
	file >> j;
	file.close();

	Ax.setKeys({ j["animation"]["Ax"]["keys"].get<std::vector<Animation::Keyframe>>()});
	Ax.label = "Ax";
	Ay.setKeys( { Animation::Keyframe(0,-140) } );
	Ay.label = "Ay";
	moviefile = j["movie"]["filename"].get<std::string>();
	startTime = j.value("startTime", startTime);
	endTime = j.value("endTime", endTime);
	deformFactor = j.value("deformFactor", deformFactor);
	extensionLength = j.value("extensionLength", extensionLength);

	double p1x = j["sourcePath"]["segment1"]["point"]["x"];
	double p1y = j["sourcePath"]["segment1"]["point"]["y"];
	sourcePath.getSegments()[1].point(glm::vec2(p1x, p1y));
	double h1x = j["sourcePath"]["segment1"]["handleIn"]["x"];
	double h1y = j["sourcePath"]["segment1"]["handleIn"]["y"];
	sourcePath.getSegments()[1].handleIn(glm::vec2(h1x, h1y));
}

void Grassblade::importKeysFromAE() {
	std::string filename{ ofToDataPath("projects/aeKeyframesData.txt") };
	std::vector<Animation::AnimationCurve> curves = Animation::Translators::AEAnimationData::import(filename);

	cout << curves.size() << " anim curves imported from: " << filename << endl;

	Ax = curves[0]; // TODO: the order has not been tested yet... !!!
	Ay = curves[1];
}

void Grassblade::importKeysFromMaya() {
	std::string filename{ "C:/Users/andris/Desktop/SelfRelativity/Production/anim_6923_v05.anim" };
	std::vector<Animation::AnimationCurve> curves = Animation::Translators::MayaAnim::import(filename);

	cout << curves.size() << " anim curves imported from: " << filename << endl;

	for (auto & key : curves[0].keys()) {
		key.value(key.value() * 100);
	}
	for (auto & key : curves[1].keys()) {
		key.value(key.value() * -100);
	}

	Ax = curves[0];
	Ay = curves[1];
}

void Grassblade::tick() {
	/* VIEWER */
	ImGui::Begin("Viewer");
	Im2D::BeginViewer("viewport");

	if (Im2D::DragPath("source", &sourcePath)) {
		auto P = sourcePath.getFirstSegment().point();
		if (autokey) {
			Ax.setValueAtFrame(P.x, F);
			Ay.setValueAtFrame(P.y, F);
		}
	};

	Im2D::EndViewer();
	ImGui::End();

	/* INSPECTOR */
	ImGui::Begin("Inspector");
	if (ImGui::Button("save")) {
		save();
	}

	if (ImGui::Button("importKeysFromAE")) {
		importKeysFromAE();
	}

	if (ImGui::Button("importKeysMaya")) {
		importKeysFromMaya();
	}

	if (ImGui::Button("copy from source")) {
		targetPath = Paper::Path(sourcePath);
	}

	if (ImGui::CollapsingHeader("read movie", ImGuiTreeNodeFlags_DefaultOpen)) {
		std::string str = reader.getFile();
		std::vector<char> data(str.begin(), str.end());
		if (ImGui::InputText("path", data.data(), data.size())) {
			std::string str{ data.data() };
			reader.setFile(str);
		}

		if (reader.getFile().empty()) {
			ImGui::TextColored(ImColor(255, 0, 0), "- no file --");
		}
		else {
			ImGui::TextWrapped("file: %s", reader.getFile().c_str());
		}
		ImGui::Text("begin: %i, end: %i", 0, reader.getFrameCount());
		ImGui::Text("dimension: %ix%ipx", reader.getWidth(), reader.getHeight());
	}

	if (ImGui::CollapsingHeader("extend paths", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("modifie source and target path");
		ImGui::DragFloat("length", &extensionLength);
	}
	if (ImGui::CollapsingHeader("Deform", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::SliderFloat("deform", &deformFactor, 0, 1.0);
	}
	ImGui::End();

	/* TIME SLIDER */
	ImGui::Begin("TimeSlider");
	Widgets::Timeslider("Timeslider", &F, &play, &startTime, &endTime);
	ImGui::End();

	/* GRAPH EDITOR */
	ImGui::Begin("Grapheditor");
	std::vector<Animation::AnimationCurve*> curves = { &Ax, &Ay};
	Widgets::Grapheditor("GraphEditor", curves, &F);

	//// handle shortcuts
	//if (ImGui::IsKeyPressed(83/*s*/)) {
	//	for (Animation::AnimationCurve * animCurve : curves) {
	//		if (animCurve->hasKeyAtFrame(F)) {
	//			animCurve->removeKeyAtFrame(F);
	//		}
	//		else {
	//			animCurve->insertKeyAtFrame(F);
	//		}
	//	}
	//}
	ImGui::End();
	ShowRender();
	ShowPreview();
}

void Grassblade::ShowRender() {
	static int renderSize[2]{ reader.getWidth(), reader.getHeight() };
	static int sequenceStart{ startTime };
	static int sequenceEnd{ endTime };
	static double renderSequenceProgress{ 0 };
	static double renderSingleProgress{ 0 };

	ImGui::Begin("Render");
	ImGui::InputInt2("renderSize", renderSize);
	if(ImGui::CollapsingHeader("RenderSingle", ImGuiTreeNodeFlags_DefaultOpen)){
		// render single ui
		static ofImage renderSingleImage;
		static std::future<cv::Mat> renderSingleFuture;
		if (!renderSingleFuture.valid() && ImGui::Button("render single")) {
			// launch
			renderSingleFuture = std::async(std::launch::async, renderSingle, 
				F, renderSize[0], renderSize[1], 
				&reader, sourcePath, Ax, Ay, targetPath, &renderSingleProgress);
				
				//&reader, renderSize[0], renderSize[1], sourcePath, Ax, Ay, targetPath, F, &renderSingleProgress);
		}
		else if (renderSingleFuture.valid() && renderSingleFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
			// done
			cv::Mat renderedImage = renderSingleFuture.get();
			cv::Mat rgb;
			cv::cvtColor(renderedImage, rgb, CV_BGR2RGB);
			renderSingleImage.setFromPixels(rgb.data, rgb.cols, rgb.rows, ofGetImageTypeFromGLType(GL_RGB));
			renderSingleProgress = -1;
		}
		else if (renderSingleFuture.valid()) {
			// progress
			ImGui::Text("rendering single..."); ImGui::SameLine();
			ImGui::ProgressBar(renderSingleProgress, ImVec2(-1, 0));
		}
		else if (renderSingleImage.isAllocated()) {
			// display results
			ImGui::Image((void*)renderSingleImage.getTexture().getTextureData().textureID, ImVec2(192, 108));
		}
	}

	// render sequence ui
	if (ImGui::CollapsingHeader("Render Sequence", ImGuiTreeNodeFlags_DefaultOpen)) {
		static std::future<void> renderSequenceFuture;
		ImGui::InputInt("start", &sequenceStart);
		ImGui::InputInt("end", &sequenceEnd);
		if (!renderSequenceFuture.valid() && ImGui::Button("render sequence")) {
			// launch
			renderSequenceFuture = std::async(std::launch::async, renderSequence, sequenceStart, sequenceEnd, renderSize[0], renderSize[1], &reader, sourcePath, targetPath, Ax, Ay, &renderSequenceProgress);
		}
		else if (renderSequenceFuture.valid() && renderSequenceFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
			//done
			renderSequenceProgress = 1.0;
		}
		else if (renderSequenceFuture.valid()) {
			// progress
			ImGui::Text("rendering sequence..."); ImGui::SameLine();
			ImGui::ProgressBar(renderSequenceProgress, ImVec2(-1, 0));
		}
		else {
			// display results
		}
	}
	ImGui::End();
}


void Grassblade::ShowPreview() {
	// Process preview
	// get video image
	ofImage videoImage;
	videoImage = reader.getImageAtFrame(F);

	// animate source path
	sourcePath.getSegments()[0].point( glm::vec2({ Ax.getValueAtFrame(F), Ay.getValueAtFrame(F) }) );

	// extend paths
	static float extendMultiply{ 1.0 };
	ImGui::Begin("Inspector");
	ImGui::DragFloat("extend mult", &extendMultiply, 1.0, 0, 4);
	ImGui::End();
	auto extendedSourcePath = extend(sourcePath, sourcePath.getLength()*extendMultiply);
	auto extendedTargetPath = extend(targetPath, targetPath.getLength()*extendMultiply);

	// deform plate
	if (deformFactor > 0) {
		auto & mesh = plate.getMesh();
		for (auto i = 0; i < mesh.getVertices().size(); i++) {
			glm::vec2 P0 = plateMesh.getVertex(i);
			auto P1 = Field::pathToPath(extendedSourcePath, extendedTargetPath, P0);
			if (!isnan(P1.x) && !isnan(P1.y)) {
				mesh.setVertex(i, glm::vec3(lerp(P0, P1, deformFactor), 0));
			}
		}
	}

	// display
	// sync ofCamera to viewer
	ImGui::SetNextWindowBgAlpha(0.0);
	static bool ViewerOpen{ true };
	ImGui::Begin("Viewer", &ViewerOpen, ImGuiWindowFlags_NoBackground);
	Im2D::BeginViewer("viewport");
	Utils::ofSyncCameraToViewport(cam);
	Im2D::EndViewer();
	ImGui::End();

	// draw extended paths
	ImGui::Begin("Viewer", &ViewerOpen, ImGuiWindowFlags_NoBackground);
	Im2D::BeginViewer("viewport");

	//display source path
	for (auto const & curve : extendedSourcePath.getCurves()) {
		auto A = curve.segment1()->point();
		auto B = A + curve.segment1()->handleOut();
		auto D = curve.segment2()->point();
		auto C = D + curve.segment2()->handleIn();
		addBezierCurve(A, B, C, D, ImColor(255,255,255), 1);

		addPoint(A, ImColor(255,255,255), 4.0);
		addPoint(B, ImColor(255, 255, 255), 4.0);
		addPoint(C, ImColor(255, 255, 255), 4.0);
		addPoint(D, ImColor(255, 255, 255), 4.0);
		addLineSegment(A, B, ImColor(255, 255, 255), 1);
		addLineSegment(C, D, ImColor(255, 255, 255), 1);
	}

	for (auto i = 0; i < 100; i++) {
		double t = (double)i / 100;
		double offset = t * extendedSourcePath.getLength();
		addPoint(extendedSourcePath.getLocationAt(offset).point(), ImColor(255, 0, 0), 3);
	}

	//display target path
	for (auto const & curve : extendedTargetPath.getCurves()) {
		auto A = curve.segment1()->point();
		auto B = A + curve.segment1()->handleOut();
		auto D = curve.segment2()->point();
		auto C = D + curve.segment2()->handleIn();
		addBezierCurve(A, B, C, D, ImColor(0, 0, 255), 1);
	}
	Im2D::EndViewer();
	ImGui::End();

	// draw image plate
	// get video image frame
	cam.begin();
	ofSetColor(ofColor::cadetBlue);
	ofSetColor(ofColor::white);
	videoImage.bind();
	plate.draw(OF_MESH_FILL);
	ofSetColor(ofColor(255, 40));
	videoImage.unbind();
	//plate.draw(OF_MESH_WIREFRAME);
	//ofSetColor(ofColor::white);
	//ofSetColor(ofColor::white);
	cam.end();


	// show closest point
	ImGui::Begin("Viewer", &ViewerOpen, ImGuiWindowFlags_NoBackground);
	Im2D::BeginViewer("viewport");
	glm::vec2 mousePoint = Im2D::GetMousePos();
	Im2D::EndViewer();
	ImGui::End();
	double closestTime;
	glm::vec2 closestPoint;
	closestTime = extendedSourcePath.getNearestTime(mousePoint);
	closestPoint = extendedSourcePath.getPointAtTime(closestTime);
	cam.begin();
	ofDrawCircle(closestPoint, 10);
	cam.end();
}

Grassblade::~Grassblade() {
	save();
};
