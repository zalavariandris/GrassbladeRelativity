#pragma once
#include "../Project.h"
#include <imgui.h>
#include "../Im2D/Im2D.h"
#include "../Paper/Path.h"
#include "../Im2DPaper/Im2DPaper.h"
#include <memory>
#include <iostream>

class Pathdemo : Project {
private:
	Paper::Path path;
	Paper::Path path2;
public:
	Pathdemo() {
		std::cout << "length: " << path.getLength() << ", calc length: " << path.calcLength() << endl;
		path.add({
			std::make_shared<Paper::Segment>(glm::vec2(0,0), glm::vec2(0,0), glm::vec2(100, 0)),
			std::make_shared<Paper::Segment>(glm::vec2(0,100), glm::vec2(100,0), glm::vec2(0, 0))
			});
		std::cout << "length: " << path.getLength() << ", calc length: " << path.calcLength() << endl;

		path2 = Paper::Path(path);
		std::cout << "path2 length: " << path2.getLength() << std::endl;
	};
	~Pathdemo() {};
	void tick() {


		ImGui::Begin("Inspector");
		if (ImGui::Button("copy")) {
			path2 = Paper::Path(path);
		}
		ImGui::Text("path: %p", &path);
		ImGui::Text("path length %i %f calc: %f", path.LengthNeedsUpdate, path.getLength(), path.calcLength());
		ImGui::Text("path2: %p", &path2);
		ImGui::Text("path2 length: %i %f calc: %f",path2.LengthNeedsUpdate, path2.getLength(), path2.calcLength());
		ImGui::End();

		ImGui::Begin("Viewport");
		Im2D::BeginViewer("viewport");
		Im2D::DragPath("path", &path);
		Im2D::DragPath("path2", &path2);
		Im2D::EndViewer();
		ImGui::End();
	}
};