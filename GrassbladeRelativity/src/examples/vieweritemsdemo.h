#pragma once
#include "../Project.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "../Im2D/Im2D.h"
#include "../Im2D/im2d_draw.h"
#include "../Im2D/mousetools/selectandmove.h"

class Vieweritemsdemo : public Project {
public:
	void tick() {

		ImGui::Begin("Inspector");
		ImGui::End();
		ImGui::Begin("Viewport");
		ImGui::Text("any hovered %i", ImGui::IsAnyItemHovered());
		
		Im2D::BeginViewer("viewer", ImVec2(500, 500));
		static bool item1Selected{ false };
		static bool item2Selected{ false };
		static glm::vec2 pos1(0, 0);
		static glm::vec2 pos2(100, 0);
		
		Im2D::Item("item1", &pos1, &item1Selected);
		addPoint(Im2D::GetMousePos(), ImColor(255, 0, 255, 128), 10);
		Im2D::Item("item2", &pos2, &item2Selected);
		Im2D::EndViewer();

		Im2D::BeginViewer("viewer2", ImVec2(500, 500));
		static bool item3Selected{ false };
		static bool item4Selected{ false };
		static glm::vec2 pos3(0, 0);
		static glm::vec2 pos4(100, 0);

		Im2D::Item("item3", &pos3, &item3Selected);
		addPoint(Im2D::GetMousePos(), ImColor(255, 0, 255, 128), 10);
		Im2D::Item("item4", &pos4, &item4Selected);

		Im2D::EndViewer();
		ImGui::End();
	}
};