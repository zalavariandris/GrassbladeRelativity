#pragma once
#include "../Project.h"
#include <imgui.h>
#include "../Im2D/Im2D.h"
#include "../Paper/Path.h"
#include "../Im2DPaper/Im2DPaper.h"
#include <memory>
#include <iostream>
#include "../utilities.h"
#include "../Im2D/im2d_draw.h"
class Pathdemo : Project {
private:
	Paper::Path path;
	Paper::Path path2;
public:
	Pathdemo() {
		std::cout << "length: " << path.getLength() << endl;
		path.add({
			Paper::Segment(glm::vec2(0,0), glm::vec2(0,0), glm::vec2(100, 0)),
			Paper::Segment(glm::vec2(0,100), glm::vec2(100,0), glm::vec2(0, 0))
			});
		std::cout << "length: " << path.getLength() << endl;

		path2 = path;
		std::cout << "path2 length: " << path2.getLength() << std::endl;
	};
	~Pathdemo() {};
	void tick() {
		ImGui::Begin("Inspector");
		if (ImGui::Button("copy")) {
			path2 = path;
		}
		ImGui::Text("path:, %p", &path);
		bool pathClosed{ path.closed() };
		if (ImGui::Checkbox("closed", &pathClosed)) {
			path.closed(pathClosed);
		}
		ImGui::Text("path length %f", path.getLength());

		ImGui::Text("path2:, %p", &path2);

		bool path2Closed{ path2.closed() };
		if (ImGui::Checkbox("closed2", &path2Closed)) {
			path2.closed(path2Closed);
		}
		ImGui::Text("path2 length %f", path2.getLength());

		static float extendLength{ 100 };
		ImGui::DragFloat("extend length", &extendLength);
		auto extendedPath = extend(path2, extendLength);
		ImGui::End();

		ImGui::Begin("Viewport");
		Im2D::BeginViewer("viewport");
		Im2D::DragPath("path", &path);
		Im2D::DragPath("path2", &path2);
		addPath(extendedPath);
		auto points = divide(extendedPath, 10);
		for (auto point : points) {
			addPoint(point, ImColor(255,255,255), 4);
		}
		Im2D::EndViewer();
		ImGui::End();
	}
};