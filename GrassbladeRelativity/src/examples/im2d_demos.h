#pragma once
#include "../Im2D/Im2D.h"

void showViewerDemo() {
	// show viewer options
	ImGui::Begin("Single Viewer");
	//setup flags
	int flags = Im2DViewportFlags_None;
	static bool grid{ true };
	static bool independentZoom{ false };
	if (grid)
		flags = flags | Im2DViewportFlags_Grid;
	if (independentZoom)
		flags = flags | Im2DViewportFlags_AllowNonUniformZoom;


	ImGui::Checkbox("grid", &grid);
	ImGui::Checkbox("NouniformZoom", &independentZoom);
	// set size
	static int size[2]{ 0,0 };
	ImGui::DragInt2("size", size, 1.0, 0, 300);
	ImGui::SameLine();
	if (ImGui::Button("Reset"))
		size[0] = size[1] = 0;

	Im2D::ViewerBegin("viewer1", ImVec2(size[0], size[1]), flags);
	Im2D::ViewerEnd();
	ImGui::End();

	ImGui::Begin("Test Viewer");
	// test multiple view state
	if (ImGui::CollapsingHeader("Keep seperate state for each viewer", ImGuiTreeNodeFlags_DefaultOpen)) {
		Im2D::ViewerBegin("viewer1", ImVec2(100, 100));
		Im2D::ViewerEnd();
		ImGui::SameLine();
		Im2D::ViewerBegin("viewer2", ImVec2(100, 100));
		Im2D::ViewerEnd();
	}

	//Test: reenter Viewer
	if (ImGui::CollapsingHeader("ReenterViewer", ImGuiTreeNodeFlags_DefaultOpen)) {
		static glm::vec2 P{ 0,0 }, Q{ 50, 0 };
		Im2D::ViewerBegin("reenter viewer", ImVec2(200, 200));
		Im2D::DragPoint("P", &P);
		Im2D::ViewerEnd();

		Im2D::ViewerBegin("reenter viewer");
		Im2D::DragPoint("Q", &Q);
		Im2D::ViewerEnd();
	}
	ImGui::End();
}