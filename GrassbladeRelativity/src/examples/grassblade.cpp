
#include "../Widgets/Widgets.hpp"
#include "../Im2D/Im2D.h"
#include "../Im2D/im2d_draw.h"
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

Grassblade::Grassblade() {
	// create plate
	static float plateWidth{ 1920 }, plateHeight{ 1080 };
	static int plateColumns{ 10 }, plateRows{ 10 };
	plate.set(plateWidth, plateHeight, plateColumns, plateRows);
	plate.mapTexCoords(0, 1, 1, 0);
	//for (auto & vertex : plate.getMesh().getVertices()) {
	//	vertex.x += plateWidth / 2;
	//	vertex.y += plateHeight / 2;
	//}

	plateMesh = ofMesh(plate.getMesh());

	distortionTexture.allocate(300, 300, GL_RGBA32F);
	renderFbo.allocate(300, 300, GL_RGBA32F);


	// paths
	sourcePath = Paper::Path({
		std::make_shared<Paper::Segment>(glm::vec2(0,0)),
		std::make_shared<Paper::Segment>(glm::vec2(0,100), glm::vec2(0,-50), glm::vec2(0,0))
	});

	targetPath = Paper::Path({
		std::make_shared<Paper::Segment>(glm::vec2(0,0)),
		std::make_shared<Paper::Segment>(glm::vec2(0,100), glm::vec2(0,-50), glm::vec2(0,0))
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
	j["animation"]["Ax"]["keys"] = Ax.getKeys();
	j["sourcePath"]["segment1"]["point"]["x"] = sourcePath.getSegments()[1]->_point.x;
	j["sourcePath"]["segment1"]["point"]["y"] = sourcePath.getSegments()[1]->_point.y;
	j["sourcePath"]["segment1"]["handleIn"]["x"] = sourcePath.getSegments()[1]->_handleIn.x;
	j["sourcePath"]["segment1"]["handleIn"]["y"] = sourcePath.getSegments()[1]->_handleIn.y;

	std::ofstream o(filepath);
	o << std::setw(2) << j.dump() << std::endl;
	o.close();
	cout << "save:" << endl << j.dump() << endl;
}

void Grassblade::render() {

}

void Grassblade::open() {
	ofFile file;
	if (!file.open(filepath)) cout << "cannot open file" << endl;

	json j;
	file >> j;
	file.close();

	Ax.setKeys({ j["animation"]["Ax"]["keys"].get<std::vector<Animation::Key>>()});
	Ax.label = "Ax";
	Ay.setKeys( { Animation::Key(0,-140) } );
	Ay.label = "Ay";
	moviefile = j["movie"]["filename"].get<std::string>();
	startTime = j.value("startTime", startTime);
	endTime = j.value("endTime", endTime);
	deformFactor = j.value("deformFactor", deformFactor);
	extensionLength = j.value("extensionLength", extensionLength);

	double p1x = j["sourcePath"]["segment1"]["point"]["x"];
	double p1y = j["sourcePath"]["segment1"]["point"]["y"];
	sourcePath.getSegments()[1]->_point = glm::vec2(p1x, p1y);
	double h1x = j["sourcePath"]["segment1"]["handleIn"]["x"];
	double h1y = j["sourcePath"]["segment1"]["handleIn"]["y"];
	sourcePath.getSegments()[1]->_handleIn = glm::vec2(h1x, h1y);
}

void Grassblade::importKeysFromAE() {
	std::string filename{ ofToDataPath("projects/aeKeyframesData.txt") };
	std::vector<Animation::AnimCurve> curves = Animation::Translators::AEAnimationData::import(filename);

	cout << curves.size() << " anim curves imported from: " << filename << endl;

	Ax = curves[0]; // TODO: the order has not been tested yet... !!!
	Ay = curves[1];
}

void Grassblade::importKeysFromMaya() {
	std::string filename{ "C:/Users/andris/Desktop/grassrelativityfootage/anim_6923_v04.anim" };
	std::vector<Animation::AnimCurve> curves = Animation::Translators::MayaAnim::import(filename);

	cout << curves.size() << " anim curves imported from: " << filename << endl;

	for (auto & key : curves[0].getKeys()) {
		key.value *= 100;
	}
	for (auto & key : curves[1].getKeys()) {
		key.value *= -100;
	}
	Ax = curves[0];
	Ay = curves[1];
}

void Grassblade::onGui() {
	/* VIEWER */
	ImGui::Begin("Viewer");
	Im2D::ViewerBegin("viewport");

	glm::vec2 P1 = sourcePath.getSegments()[0]->_point;
	if (Im2D::DragPoint("P1", &P1)) {
		// animate
		if (autokey) {
			Ax.setValueAtFrame(P1.x, F);
			Ay.setValueAtFrame(P1.y, F);
		}
	}
	glm::vec2 h2 = sourcePath.getSegments()[1]->_point+sourcePath.getSegments()[1]->_handleIn;
	if (Im2D::DragPoint("h2", &h2)) {
		sourcePath.getSegments()[1]->_handleIn = h2 - sourcePath.getSegments()[1]->_point;
	}

	Im2D::DragPoint("p2", &sourcePath.getSegments()[1]->_point);

	Im2D::ViewerEnd();
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
		targetPath = sourcePath;
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
	Widgets::TimeSlider("TimeSlider", &F, &play, &startTime, &endTime);
	ImGui::End();

	/* GRAPH EDITOR */
	ImGui::Begin("GraphEditor");
	std::vector<Animation::AnimCurve*> curves = { &Ax, &Ay};
	Widgets::GraphEditor("GraphEditor", curves, &F);

	// handle shortcuts
	if (ImGui::IsKeyPressed(83/*s*/)) {
		for (Animation::AnimCurve * animCurve : curves) {
			if (animCurve->hasKeyAtFrame(F)) {
				animCurve->removeKeyAtFrame(F);
			}
			else {
				animCurve->insertKeyAtFrame(F);
			}
		}
	}
	ImGui::End();
}

void Grassblade::draw() {
	/* -----------------------------------
	   TIME
	   -----------------------------------*/
	if (play)
		F++;
	if (play && F > endTime)
		F = startTime;

	// sync ofCamera to viewer
	ImGui::SetNextWindowBgAlpha(0.0);
	static bool ViewerOpen{ true };
	ImGui::Begin("Viewer", &ViewerOpen, ImGuiWindowFlags_NoBackground);
	Im2D::ViewerBegin("viewport");
	Utils::ofSyncCameraToViewport(cam);
	Im2D::ViewerEnd();
	ImGui::End();

	/* -----------------------------------
	   Create target Path
	   -----------------------------------*/
	auto & currentImage = reader.getImageAtFrame(F);
	//currentImage.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);

	// animate source path
	sourcePath.getSegments()[0]->_point = glm::vec2({ Ax.getValueAtFrame(F), Ay.getValueAtFrame(F) });

	// extend paths
	Paper::Path extendedSourcePath = extend(sourcePath, extensionLength);
	Paper::Path extendedTargetPath = extend(targetPath, extensionLength);


	// draw extended paths
	cam.begin();
	ofSetColor(ofColor::white);
	Utils::ofDraw(extendedSourcePath);
	Utils::ofDraw(extendedTargetPath);
	cam.end();

	// Deform plate with paths
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

	//// display deformation field
	//ImGui::Begin("Viewer");
	//Im2D::ViewerBegin("viewport");
	//addField({ sourcePath.getSegments()[0]->_point }, [&](glm::vec2 P)->glm::vec2 {
	//	return Field::pathToPath(extendedSourcePath, extendedTargetPath, P);
	//});
	//Im2D::ViewerEnd();
	//ImGui::End();

	// draw image plate
	cam.begin();
	ofSetColor(ofColor::cadetBlue);
	ofSetColor(ofColor::white);
	currentImage.bind();
	plate.draw(OF_MESH_FILL);
	ofSetColor(ofColor(255, 40));
	currentImage.unbind();
	//plate.draw(OF_MESH_WIREFRAME);
	//ofSetColor(ofColor::white);
	Utils::ofDraw(sourcePath);
	Utils::ofDraw(targetPath);
	cam.end();


	// show closest point
	ImGui::Begin("Viewer", &ViewerOpen, ImGuiWindowFlags_NoBackground);
	Im2D::ViewerBegin("viewport");
	glm::vec2 mousePoint = Im2D::GetMousePos();
	Im2D::ViewerEnd();
	ImGui::End();
	double closestTime;
	glm::vec2 closestPoint;
	closestTime = extendedSourcePath.getNearestTime(mousePoint);
	closestPoint = extendedSourcePath.getPointAtTime(closestTime);
	cam.begin();
	ofDrawCircle(closestPoint, 10);
	cam.end();

	onGui();

	ImGui::Begin("Render");
	int distortion_size[2]{ distortionTexture.getWidth(), distortionTexture.getHeight() };
	if (ImGui::DragInt2("distortion size", distortion_size, 1.0, 2, 1920)) {
		distortionTexture.allocate(distortion_size[0], distortion_size[1], GL_RGB32F);
	}
	int renderfbo_size[2]{ renderFbo.getWidth(), renderFbo.getHeight() };
	if (ImGui::DragInt2("renderfbo size", renderfbo_size, 1.0, 2, 1920)) {
		renderFbo.allocate(renderfbo_size[0], renderfbo_size[1], GL_RGB32F);
	}

	//ASYNC RENDER
	Rect reader_rect({ -1920 / 2, -1080 / 2 }, 1920, 1080);
	Rect distortion_rect({ 0,0 }, distortionTexture.getWidth(), distortionTexture.getHeight());
	static double renderProgress{ 0 };
	static std::future<ofFloatPixels> result;
	auto renderasync = [this, distortion_rect, reader_rect, extendedTargetPath, extendedSourcePath]() {
		const int w = distortion_rect.width;
		const int h = distortion_rect.height;
		const int c = 4;
		std::vector<float> pixels(w*h*c);
		for (int i = 0; i < w * h; i++) {
			int x = i % w;
			int y = i / w;

			auto P = Rect::map({ x, y }, distortion_rect, reader_rect);
			glm::vec2 pos = Field::pathToPath(extendedTargetPath, extendedSourcePath, P);

			// map from 1080p to [0-1]
			pos = Rect::map(pos, reader_rect, { {0,0},1,1 });
			glm::vec4 color = glm::vec4(pos[0], pos[1], 0, 1);

			// fill data
			for (auto j = 0; j < c; j++) {
				pixels[i * c + j] = color[j];
			}

			renderProgress = (double)i / (w*h);
		}

		// upload data to GPU
		ofFloatPixels pix;
		pix.setFromPixels(pixels.data(), w, h, ofGetImageTypeFromGLType(GL_RGBA32F));
		
		//delete[] data;
		return pix;
	};

	if (!result.valid() && ImGui::Button("launch async render")) {
		result = std::async(std::launch::async, renderasync);
	}
	if(result.valid()) {
		ImGui::Text("rendering... %.0f%s", renderProgress*100, "%");
	}
	if (result.valid() && result.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
		auto pix = result.get();
		distortionTexture.loadData(pix);
		distort(reader.getImageAtFrame(F).getTexture(), distortionTexture, &renderFbo);
	}

	ImGui::Image((void*)distortionTexture.getTextureData().textureID, ImVec2(192, 108));
	ImGui::Image((void*)renderFbo.getTexture().getTextureData().textureID, ImVec2(192, 108));
	ImGui::End();
};

Grassblade::~Grassblade() {
	save();
};

void showGrassblade() {
	static Grassblade grass;
	grass.draw();
}
