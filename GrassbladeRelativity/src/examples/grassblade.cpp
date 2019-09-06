
#include "../Widgets/Widgets.hpp"
#include "../Im2D/Im2D.h"
#include "../Im2D/im2d_draw.h"
#include "../Paper/Path.h"
#include "../utilities.h"
#include "glm/ext.hpp"
#include "../Reader.h"

#include "ofAppRunner.h"
#include "ofMain.h"
#include <json.hpp>

#include "grassblade.h"

using json = nlohmann::json;

namespace Field {
	glm::vec2 curveToRect(Paper::Curve const & curve, glm::vec2 uv) {
		double distance = uv.x;
		double t = uv.y;

		glm::vec2 P = curve.getPointAtTime(t);
		glm::vec2 normal = curve.getNormalAtTime(t);

		glm::vec2 xy = P + normal * distance;
		return xy;
	}

	glm::vec2 pathToRect(Paper::Path const & path, glm::vec2 uv) {
		double distance = uv.x;
		double t = uv.y;

		Paper::CurveLocation loc = path.getLocationAtTime(t);
		glm::vec2 P = loc._point;
		glm::vec2 normal = loc._normal;

		glm::vec2 xy = P + normal * distance;
		return xy;
	}

	glm::vec2 rectToCurve(Paper::Curve const & curve, glm::vec2 xy) {
		double t = curve.getNearestTime(xy);
		auto Q = curve.getPointAtTime(t);
		double distance = glm::distance(xy, Q);

		glm::vec2 P = curve.getPointAtTime(t);
		glm::vec2 normal = curve.getNormalAtTime(t);

		double dot = glm::dot(glm::normalize(xy - P), glm::normalize(normal));
		glm::vec2 uv(dot > 0 ? distance : -distance, t);
		return uv;
	}

	glm::vec2 rectToPath(Paper::Path const & path, glm::vec2 xy) {
		double t = path.getNearestTime(xy);
		auto Q = path.getPointAtTime(t);
		double distance = glm::distance(xy, Q);

		glm::vec2 P = path.getPointAtTime(t);
		glm::vec2 normal = path.getNormalAtTime(t);

		double dot = glm::dot(glm::normalize(xy - P), glm::normalize(normal));
		glm::vec2 uv(dot > 0 ? distance : -distance, t);
		return uv;
	}

	glm::vec2 pathToPath(Paper::Path const & source, Paper::Path const & target, glm::vec2 P0) {
		glm::vec2 uv = Field::rectToPath(source, P0);
		glm::vec2 P1 = Field::pathToRect(target, uv);
		return P1;
	}
}

namespace Operators {
	class Extend {
		std::shared_ptr<Paper::Path> inputPath;
		double length;
		std::shared_ptr<Paper::Path> outputPath;

		Extend(std::shared_ptr<Paper::Path> path, double length);

		void evaluate() {

		}

		void onSceneGui() {

		}

		void onPropertiesGui() {

		}
	};
};

namespace {
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
			addArrow(P, field(P), ImColor(255, 50, 255, 180));
		}
	}

	//glm::vec2 lerp(glm::vec2 a, glm::vec2 b, glm::vec2 t) {
	//	return a + t * (b - a);
	//};
}

/* Serialization helpers*/
namespace Operators {
	class PathThroughPoints {
	private:
		Paper::Path m_path;
	public:
		PathThroughPoints(std::vector<glm::vec2> points) {
			m_path.add({
				std::make_shared<Paper::Segment>(points[0]),
				std::make_shared<Paper::Segment>(points[1], (points[0] - points[2])*0.25, (points[2] - points[0])*0.25),
				std::make_shared<Paper::Segment>(points[2])
				});
		}

		PathThroughPoints & gui() {
			// gui
			ImGui::Begin("Properties");
			if (ImGui::CollapsingHeader("PathThroughPoints", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::Text("smooth path from points");
			}
			ImGui::End();
			return *this;
		}

		PathThroughPoints &  draw() {
			Utils::ofDraw(m_path);
			return *this;
		}

		Paper::Path & get() {
			return m_path;
		}
	};
}

namespace deserialize {
	std::vector<glm::vec2> points(json const & j) {
		std::vector<glm::vec2> points(j.size());
		for (auto i = 0; i < j.size(); i++) {
			double x = j[i][0].get<double>();
			double y = j[i][1].get<double>();
			points[i] = glm::vec2(x, y);
		}
		return points;
	}
}

namespace serialize {
	json points(std::vector<glm::vec2> points) {
		json j;
		for (auto i = 0; i < points.size(); i++) {
			double x = points[i].x;
			double y = points[i].y;
			j.push_back({x, y});
		}
		return j;
	}
}

namespace Animation {
	void to_json(json& j, const Key& key) {
		j = { key.frame ,  key.value };
	};

	void from_json(const json& j, Key& key) {
		key.frame = j.at(0).get<double>();
		key.value = j.at(1).get<double>();
	}
}



Grassblade::Grassblade() {

};

void Grassblade::show() {

};

Grassblade::~Grassblade() {

};


json open(std::string const & filename) {

	// read file to json
	std::ifstream i(filename);
	json j;
	i >> j;
	return j;
}

void showGrassblade() {
	/* -----------------------------------
	   TIME
	   -----------------------------------*/
	   // model
	static int F{ 0 }, begin{ 0 }, end{ 99 };
	static bool play{ false };

	// logic 
	if (play)
		F++;
	if (play && F > end)
		F = begin;

	/*
	 * LOAD FROM FILE
	 */
	static json j = open("bin/data/projects/grassblade.json");


	static std::vector<glm::vec2> P1 = deserialize::points(j["targetPoints"]); // j["targetPoints"].get<std::vector<glm::vec2>>(); // TODO: load directly from json
	static Animation::AnimCurve Ax{j["animation"]["Ax"]["keys"].get<std::vector<Animation::Key>>(),"Ax" };
	//static Animation::AnimCurve Ax{ { Animation::Key(0,0) } ,"Ax" };
	static Animation::AnimCurve // TODO: Load all keys from file
		Ay{ { Animation::Key(0,-140) } ,"Ay" },
		Bx{ { Animation::Key(0,0)} ,"Bx" },
		By{ { Animation::Key(0,-30)} ,"By" },
		Cx{ { Animation::Key(0,0)} ,"Cx" },
		Cy{ { Animation::Key(0,125)} ,"Cy" };

	static std::string file = j["movie"]["filename"].get<std::string>();

	// save json to file
	ImGui::Begin("Properties");
	if (ImGui::Button("save")) {
		j["targetPoints"] = serialize::points(P1);
		j["animation"]["Ax"]["keys"] = Ax.getKeys();
		j["movie"]["filename"] = file;

		std::ofstream o("bin/data/projects/grassblade.json");
		o << std::setw(2) << j.dump() << std::endl;
		o.close();
		cout << "save:" << endl << j.dump() << endl;
	}
	ImGui::End();

	/*
	 *layout windows
	 */
	int propertiesWidth = 200;
	int timeLineHeight = 80;
	float verticalRatio = 0.75;
	int menuBarHeight = 18;
	int viewportWidth = ofGetWidth() - menuBarHeight;
	int viewportHeight = ofGetHeight();

	ImGui::SetNextWindowPos(ImVec2(viewportWidth - propertiesWidth, menuBarHeight));
	ImGui::SetNextWindowSize(ImVec2(200, viewportHeight));
	ImGui::Begin("Properties"); ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(0, menuBarHeight));
	ImGui::SetNextWindowSize(ImVec2(viewportWidth - 200, (viewportHeight - timeLineHeight)* verticalRatio));
	ImGui::SetNextWindowBgAlpha(0.0);
	ImGui::Begin("Viewer"); ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(0, (viewportHeight - timeLineHeight) * verticalRatio + menuBarHeight));
	ImGui::SetNextWindowSize(ImVec2(viewportWidth - 200, timeLineHeight));
	ImGui::Begin("TimeSlider"); ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(0, (viewportHeight - timeLineHeight) *verticalRatio + timeLineHeight + menuBarHeight));
	ImGui::SetNextWindowSize(ImVec2(viewportWidth - 200, (viewportHeight - timeLineHeight) * (1 - verticalRatio)));
	ImGui::Begin("GraphEditor"); ImGui::End();

	// sync ofCamera to viewer
	static ofCamera cam;
	ImGui::Begin("Viewer");
	Im2D::ViewerBegin("viewport");
	Utils::ofSyncCameraToViewport(cam);
	Im2D::ViewerEnd();
	ImGui::End();

	// Gui
	ImGui::Begin("TimeSlider");
	Widgets::TimeSlider("TimeSlider", &F, &play, &begin, &end);
	ImGui::End();


	/* -----------------------------------
	   Create target points
	   -----------------------------------*/


	ImGui::Begin("Properties");
	if (ImGui::CollapsingHeader("create target points", ImGuiTreeNodeFlags_DefaultOpen)) {
		for (auto i = 0; i < P1.size(); i++) {
			DragVec2(std::to_string(i).c_str(), &P1[i]);
		}
	}
	ImGui::End();

	ImGui::Begin("Viewer");
	Im2D::ViewerBegin("viewport");
	for (auto i = 0; i < P1.size(); i++) {
		Im2D::DragPoint(std::to_string(i).c_str(), &P1[i]);
	}
	Im2D::ViewerEnd();
	ImGui::End();


	/* -----------------------------------
	   Create target Path
	   -----------------------------------*/
	Paper::Path path1;
	path1.add(Paper::Segment(P1[0]));
	path1.add(Paper::Segment(P1[1], (P1[0] - P1[2])*0.25, (P1[2] - P1[0])*0.25));
	path1.add(Paper::Segment(P1[2]));

	// gui
	ImGui::Begin("Properties");
	if (ImGui::CollapsingHeader("create target path", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("smooth path from points");
	}
	ImGui::End();

	// draw target path
	cam.begin();
	Utils::ofDraw(path1);
	cam.end();

	/* -----------------------------------
	   Animation
	   -----------------------------------*/
	ImGui::Begin("Properties");
	if (ImGui::CollapsingHeader("create animcurves", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("Ax %g", Ax.getValueAtFrame(F));
		ImGui::Text("Ay %g", Ay.getValueAtFrame(F));
		ImGui::Text("Bx %g", Bx.getValueAtFrame(F));
		ImGui::Text("By %g", By.getValueAtFrame(F));
		ImGui::Text("Cx %g", Cx.getValueAtFrame(F));
		ImGui::Text("Cy %g", Cy.getValueAtFrame(F));
	}
	ImGui::End();

	// edit animation curves as points
	ImGui::Begin("Viewer");
	Im2D::ViewerBegin("viewport");
	glm::vec2 A0{ Ax.getValueAtFrame(F), Ay.getValueAtFrame(F) };
	glm::vec2 B0{ Bx.getValueAtFrame(F), By.getValueAtFrame(F) };
	glm::vec2 C0{ Cx.getValueAtFrame(F), Cy.getValueAtFrame(F) };
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

	// edit curves in graph editor
	ImGui::Begin("GraphEditor");
	std::vector<Animation::AnimCurve*> curves = { &Ax, &Ay, &Bx, &By, &Cx, &Cy };
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

	/* -----------------------------------
	   Create animated source path
	   -----------------------------------*/
	Paper::Path path0;
	path0.add({
		std::make_shared<Paper::Segment>(A0),
		std::make_shared<Paper::Segment>(B0, (A0 - C0)*0.25, (C0 - A0)*0.25),
		std::make_shared<Paper::Segment>(C0)
		});

	ImGui::Begin("Properties");
	if (ImGui::CollapsingHeader("create source path", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("smooth path from points");
	}
	ImGui::End();

	/* -----------------------------------
	   Read movie
	   -----------------------------------*/
	static Reader reader(file);
	auto & currentImage = reader.getImageAtFrame(F);
	currentImage.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);

	ImGui::Begin("Properties");
	if (ImGui::CollapsingHeader("read movie", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::TextWrapped("file: %s", !reader.getFile().empty() ? reader.getFile().c_str() : "-no file-");
		ImGui::Text("begin: %i, end: %i", 0, reader.getFrameCount());
		ImGui::Text("dimension: %ix%ipx", reader.getWidth(), reader.getHeight());
	}
	ImGui::End();

	// draw movie
	//{
	//	ImGui::Begin("Viewer");
	//	Im2D::ViewerBegin("viewport");
	//	cam.begin();
	//	ofSetColor(ofColor::white);
	//	currentImage.draw(0, 0);
	//	cam.end();
	//	Im2D::ViewerEnd();
	//	ImGui::End();
	//}

	/* -----------------------------------
	   Create Plate
	   -----------------------------------*/

	static ofPlanePrimitive plate; //TODO: load from file
	static float plateWidth{ 720 }, plateHeight{ 405 };
	static int plateColumns{ 10 }, plateRows{ 10 };
	plate.set(plateWidth, plateHeight, plateColumns, plateRows);
	plate.mapTexCoords(0, 1, 1, 0);

	ImGui::Begin("Properties");
	if (ImGui::CollapsingHeader("create plate", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat("width", &plateWidth);
		ImGui::DragFloat("height", &plateHeight);
		ImGui::DragInt("columns", &plateColumns);
		ImGui::DragInt("rows", &plateRows);
	}
	ImGui::End();

	// draw plate
	//{
	//	ImGui::Begin("Viewer");
	//	Im2D::ViewerBegin("viewport");
	//	cam.begin();
	//	ofSetColor(ofColor::cadetBlue);
	//	plate.draw(OF_MESH_FILL);
	//	ofSetColor(ofColor::cadetBlue);
	//	plate.draw(OF_MESH_WIREFRAME);
	//	cam.end();
	//	Im2D::ViewerEnd();
	//	ImGui::End();
	//}

	/* -----------------------------------
	   Extend paths
	   -----------------------------------*/
	static float length{ 250 }; //TODO: load from file
	ImGui::Begin("Properties");
	if (ImGui::CollapsingHeader("extend paths", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("modifie source and target path");
		ImGui::DragFloat("length", &length);
	}
	ImGui::End();

	auto firstLocation0 = path0.getLocationAtTime(0);
	auto lastLocation0 = path0.getLocationAtTime(1.0);
	path0.insert(0, std::make_shared<Paper::Segment>(firstLocation0._point - firstLocation0._tangent*length));
	path0.add(std::make_shared<Paper::Segment>(lastLocation0._point + lastLocation0._tangent*length));

	auto firstLocation1 = path1.getLocationAtTime(0);
	auto lastLocation1 = path1.getLocationAtTime(1.0);
	path1.insert(0, std::make_shared<Paper::Segment>(firstLocation1._point - firstLocation1._tangent*length));
	path1.add(std::make_shared<Paper::Segment>(lastLocation1._point + lastLocation1._tangent*length));

	// draw extended paths
	ImGui::Begin("Viewer");
	Im2D::ViewerBegin("viewport");
	cam.begin();
	ofSetColor(ofColor::white);
	Utils::ofDraw(path0);
	Utils::ofDraw(path1);
	cam.end();
	Im2D::ViewerEnd();
	ImGui::End();

	/* -----------------------------------
	   Deform plate with paths
	   -----------------------------------*/
	static float deform{ 1.0 }; //TODO: load from file
	ImGui::Begin("Properties");
	if (ImGui::CollapsingHeader("Deform", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::SliderFloat("deform", &deform, 0, 1.0);

	}
	ImGui::End();

	ImGui::Begin("Viewer");
	Im2D::ViewerBegin("viewport");

	addField({ A0, B0, C0 }, [&](glm::vec2 P)->glm::vec2 {
		return Field::pathToPath(path0, path1, P);
	});
	Im2D::ViewerEnd();
	ImGui::End();

	if (deform > 0) {
		auto mesh = plate.getMeshPtr();
		for (auto i = 0; i < mesh->getVertices().size(); i++) {
			glm::vec2 P0 = mesh->getVertex(i);
			auto P1 = Field::pathToPath(path0, path1, P0);
			if (!isnan(P1.x) && !isnan(P1.y)) {
				mesh->setVertex(i, glm::vec3(lerp(P0, P1, deform), 0));
			}
		}
	}

	cam.begin();
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

	//save("grassblade.json");
}
