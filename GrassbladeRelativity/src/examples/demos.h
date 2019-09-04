#pragma once
#include "imgui.h"
#include "Im2D/Im2D.h"
#include "Paper/Path.h"
#include <glm/glm.hpp>
#include "Im2D/Im2D.h"
#include "Im2D/im2d_draw.h"
#include "Animation/AnimCurve.h"
#include "Widgets/Widgets.hpp"

#include "utilities.h"
/* Demos */

void showReadDemo() {
	//auto file = "C:/Users/andris/Desktop/grassfieldwind.mov";
	auto file = "C:/Users/andris/Pictures/2019-08/IMG_6926.MOV";
	static Reader reader(file);

	ImGui::TextWrapped("file: %s", !reader.getFile().empty() ? reader.getFile().c_str() : "-no file-");
	ImGui::Text("dimension: %ix%ipx", reader.getWidth(), reader.getHeight());
	ImGui::Separator();

	int start{ 0 };
	int end{ reader.getFrameCount()-1 };
	static int F{ 0 };
	static bool play{ false };
	Widgets::TimeSlider("timeslider", &F, &play, &start, &end);
	
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

void showAnimationDemo() {
	/* Model */
	static glm::vec2 P;
	static int F{ 0 };
	static int begin{ 0 };
	static int end{ 500 };
	static bool play{ false };
	static Animation::AnimCurve animCurveX({
		Animation::Key(0,0),
		Animation::Key(50,50),
		Animation::Key(100,100),
		Animation::Key(150,150),
		Animation::Key(200,200),
		Animation::Key(250,250)
		});
	animCurveX.label = "X";

	static Animation::AnimCurve animCurveY({
		Animation::Key(0,250),
		Animation::Key(50,200),
		Animation::Key(100,50),
		Animation::Key(150,100),
		Animation::Key(200,50),
		Animation::Key(250,50)
		});
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
	Utils::addTrajectory(animCurveX, animCurveY, F-framesBefore, F+framesAfter);

	//add plot
	

	Im2D::ViewerEnd();
	ImGui::End();

	/* control time with a __TimeSlider__ */
	ImGui::Begin("TimeSlider");
	Widgets::TimeSlider("TimeSlider", &F, &play, &begin, &end);
	ImGui::End();

	/* Show animCurves in the __GraphEditor__ */
	ImGui::Begin("GraphEditor");
	std::vector<Animation::AnimCurve*> curves{ &animCurveX, &animCurveY };

	// Toolbar
	if (ImGui::Button("Insert Key At current frame")) {
		for (Animation::AnimCurve * animCurve : curves) {
			animCurve->insertKeyAtFrame(F);
		}
	}
	if (ImGui::Button("Delete Selected Keys")) {
		for (Animation::AnimCurve * animCurve : curves) {
			auto & keys = animCurve->getKeys();
			for (int i = keys.size() - 1; i >= 0; i--) {
				if (keys.at(i).selected) {
					keys.erase(keys.begin() + i);
				}
			}
		}
	}

	// Editor space
	Widgets::GraphEditor("GraphEditor", {&animCurveX, &animCurveY}, &F);

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
	Widgets::TimeSlider("TimeSlider", &F, &play, &begin, &end);
	ImGui::End();

	/* CONTROL POINTS */
	// model
	static glm::vec2 A0{ 20, -140 }, B0{-20, -30}, C0{ -30, 125 };
	static glm::vec2 A1{ 70, -140 }, B1{ 30, -30 }, C1{ 20, 125 };

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
	auto file = "C:/Users/andris/Desktop/grassfieldwind.mov";
	//auto file = "C:/Users/andris/Pictures/2019-08/IMG_6926.MOV";
	static Reader reader(file);
	auto & currentImage = reader.getImageAtFrame(F);

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
	ImGui::Begin("Outliner");
	if (ImGui::CollapsingHeader("CreatePlate", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Checkbox("deform", &deform);
		ImGui::DragFloat("width", &plateWidth);
		ImGui::DragFloat("height", &plateHeight);
		ImGui::DragInt("columns", &plateColumns);
		ImGui::DragInt("rows", &plateRows);
		
	}
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
	ImGui::Begin("Outliner");
	if (ImGui::CollapsingHeader("ReadFile", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::TextWrapped("file: %s", !reader.getFile().empty() ? reader.getFile().c_str() : "-no file-");
		ImGui::Text("begin: %i, end: %i", 0, reader.getFrameCount());
		ImGui::Text("dimension: %ix%ipx", reader.getWidth(), reader.getHeight());
	}
	ImGui::End();

	/* animate source path */
	static Animation::AnimCurve
		Ax{ { Animation::Key(0,0) } ,"Ax"},
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
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(200, ofGetHeight()-100));
	ImGui::Begin("Outliner"); ImGui::End();
	
	ImGui::SetNextWindowPos(ImVec2(200, 0));
	ImGui::SetNextWindowSize(ImVec2((ofGetWidth()-200) * 2/3, ofGetHeight() -100));
	ImGui::Begin("Grassblade"); ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(200+(ofGetWidth() - 200) *2/3, 0));
	ImGui::SetNextWindowSize(ImVec2((ofGetWidth() - 200)*1/3, ofGetHeight() -100));
	ImGui::Begin("GraphEditor"); ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(0, ofGetHeight() -100));
	ImGui::SetNextWindowSize(ImVec2(ofGetWidth(), 100));
	ImGui::Begin("TimeSlider"); ImGui::End();	
}

void showDemos() {
	
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
	//ImGui::ShowDemoWindow();
}