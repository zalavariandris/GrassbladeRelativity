#pragma once
#include "../Widgets/Widgets.hpp"
#include "../Im2D/Im2D.h"
#include "../Paper/Path.h"
#include "../utilities.h"
#include "glm/ext.hpp"
#include "../Reader.h"

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

bool DragVec2(const char * label, glm::vec2 * P) {
	float v[2]{ P->x, P->y };
	if (ImGui::DragFloat2(label, v)) {
		P->x = v[0]; P->y = v[1];
		return true;
	}
	return false;
}

void addField(std::vector<glm::vec2> points, std::function<glm::vec2(glm::vec2)> field) {
	for (auto P : points) {
		addArrow(P, field(P), ImColor(255,50,255, 180));
	}
}

namespace Operators {
	class Extend {
		std::shared_ptr<Path> inputPath;
		double length;
		std::shared_ptr<Path> outputPath;

		Extend(std::shared_ptr<Path> path, double length);

		void evaluate() {

		}
		
		void onSceneGui() {

		}

		void onPropertiesGui() {

		}
	};
};

void showGrassblade() {

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
	Widgets::TimeSlider("TimeSlider", &F, &play, &begin, &end);
	ImGui::End();

	/* CONTROL POINTS */
	// model
	static glm::vec2 A0{ 20, -140 }, B0{ -20, -30 }, C0{ -30, 125 };
	static glm::vec2 A1{ 70, -140 }, B1{ 30, -30 }, C1{ 20, 125 };

	// Gui
	ImGui::Begin("Properties");
	if (ImGui::CollapsingHeader("Targt points", ImGuiTreeNodeFlags_DefaultOpen)) {
		DragVec2("A1", &A1);
		DragVec2("B1", &B1);
		DragVec2("C1", &C1);
	}
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
		std::make_shared<Segment>(A0),
		std::make_shared<Segment>(B0, (A0 - C0)*0.25, (C0 - A0)*0.25),
		std::make_shared<Segment>(C0)
		});

	Path path1;
	path1.add({
		std::make_shared<Segment>(A1),
		std::make_shared<Segment>(B1, (A1 - C1)*0.25, (C1 - A1)*0.25),
		std::make_shared<Segment>(C1)
		});

	//extend paths
	static float length{ 250 };
	ImGui::Begin("Properties");
	if (ImGui::CollapsingHeader("ExtendPath", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat("length", &length);
	}
	ImGui::End();

	auto firstLocation0 = path0.getLocationAtTime(0);
	auto lastLocation0 = path0.getLocationAtTime(1.0);
	path0.insert(0, std::make_shared<Segment>(firstLocation0._point - firstLocation0._tangent*length));
	path0.add(std::make_shared<Segment>(lastLocation0._point + lastLocation0._tangent*length));

	auto firstLocation1 = path1.getLocationAtTime(0);
	auto lastLocation1 = path1.getLocationAtTime(1.0);
	path1.insert(0, std::make_shared<Segment>(firstLocation1._point - firstLocation1._tangent*length));
	path1.add(std::make_shared<Segment>(lastLocation1._point + lastLocation1._tangent*length));

	/* read the movie file */
	//auto file = "C:/Users/andris/Desktop/grassfieldwind.mov";
	auto file = "C:/Users/andris/Pictures/2019-08/IMG_6926.MOV";
	static Reader reader(file);
	auto & currentImage = reader.getImageAtFrame(F);
	currentImage.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);

	// draw OF
	static ofCamera cam;
	ImGui::Begin("Grassblade");
	Im2D::ViewerBegin("viewport");
	Utils::ofSyncCameraToViewport(cam);
	Im2D::ViewerEnd();
	ImGui::End();
	cam.begin();

	// deform mesh
	static bool deform{ true };
	static ofPlanePrimitive plate;
	static float plateWidth{ 720 }, plateHeight{ 405 };
	static int plateColumns{ 10 }, plateRows{ 10 };
	ImGui::Begin("Properties");
	if (ImGui::CollapsingHeader("CreatePlate", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Checkbox("deform", &deform);
		ImGui::DragFloat("width", &plateWidth);
		ImGui::DragFloat("height", &plateHeight);
		ImGui::DragInt("columns", &plateColumns);
		ImGui::DragInt("rows", &plateRows);

	}
	ImGui::End();

	ImGui::Begin("Grassblade");
	Im2D::ViewerBegin("viewport");
	addField({ A0, B0, C0 }, [&](glm::vec2 P)->glm::vec2{
		return Field::pathToPath(path0, path1, P);
	});
	Im2D::ViewerEnd();
	ImGui::End();

	plate.set(plateWidth, plateHeight, plateColumns, plateRows); //reset mesh
	plate.mapTexCoords(0, 1, 1, 0);
	if (deform) {
		auto mesh = plate.getMeshPtr();
		for (auto i = 0; i < mesh->getVertices().size(); i++) {
			glm::vec2 P0 = mesh->getVertex(i);
			auto P1 = Field::pathToPath(path0, path1, P0);
			if (!isnan(P1.x) && !isnan(P1.y)) {
				mesh->setVertex(i, glm::vec3(P1, 0));
			}
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
	Utils::ofDraw(path0);
	Utils::ofDraw(path1);

	cam.end();

	/* control time */
	ImGui::Begin("Properties");
	if (ImGui::CollapsingHeader("ReadFile", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::TextWrapped("file: %s", !reader.getFile().empty() ? reader.getFile().c_str() : "-no file-");
		ImGui::Text("begin: %i, end: %i", 0, reader.getFrameCount());
		ImGui::Text("dimension: %ix%ipx", reader.getWidth(), reader.getHeight());
	}
	ImGui::End();

	/* animate source path */
	static Animation::AnimCurve
		Ax{ { Animation::Key(0,0) } ,"Ax" },
		Ay{ { Animation::Key(0,-140) } ,"Ay" },
		Bx{ { Animation::Key(0,0)} ,"Bx" },
		By{ { Animation::Key(0,-30)} ,"By" },
		Cx{ { Animation::Key(0,0)} ,"Cx" },
		Cy{ { Animation::Key(0,125)} ,"Cy" };

	ImGui::Begin("Grassblade");
	Im2D::ViewerBegin("viewport");
	if (Im2D::DragPoint("A0", &A0)) {
		Ax.setValueAtFrame(A0.x, F);
		Ay.setValueAtFrame(A0.y, F);
	}
	if (Im2D::DragPoint("B0", &B0)) {
		Bx.setValueAtFrame(B0.x, F);
		By.setValueAtFrame(B0.y, F);
	}
	if (Im2D::DragPoint("C0", &C0)) {
		Cx.setValueAtFrame(C0.x, F);
		Cy.setValueAtFrame(C0.y, F);
	}
	Im2D::ViewerEnd();
	ImGui::End();

	ImGui::Begin("GraphEditor");
	std::vector<Animation::AnimCurve*> curves = { &Ax, &Ay, &Bx, &By, &Cx, &Cy };
	Widgets::GraphEditor("GraphEditor", curves, &F);
	ImGui::End();

	// Handle shortcuts
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

	A0 = { Ax.getValueAtFrame(F), Ay.getValueAtFrame(F) };
	B0 = { Bx.getValueAtFrame(F), By.getValueAtFrame(F) };
	C0 = { Cx.getValueAtFrame(F), Cy.getValueAtFrame(F) };

	/* layout windows*/
	int propertiesWidth = 200;
	int timeLineHeight = 80;
	float verticalRatio = 0.75;
	ImGui::SetNextWindowPos(ImVec2(ofGetWidth()- propertiesWidth, 0));
	ImGui::SetNextWindowSize(ImVec2(200, ofGetHeight()));
	ImGui::Begin("Properties"); ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(ofGetWidth()-200, (ofGetHeight() - timeLineHeight)* verticalRatio));
	ImGui::Begin("Grassblade"); ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(0, (ofGetHeight() - timeLineHeight) * verticalRatio));
	ImGui::SetNextWindowSize(ImVec2(ofGetWidth()-200, timeLineHeight));
	ImGui::Begin("TimeSlider"); ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(0, (ofGetHeight() - timeLineHeight) *verticalRatio+timeLineHeight));
	ImGui::SetNextWindowSize(ImVec2(ofGetWidth()-200, (ofGetHeight() - timeLineHeight) * (1-verticalRatio)));
	ImGui::Begin("GraphEditor"); ImGui::End();
}
