#pragma once
#include "Im2D.h"

void showViewerDemo() {
	ImGui::Begin("Single Viewer in a window");

	int flags = Im2DViewportFlags_None;
	static bool grid{ true };
	static bool independentZoom{ false };
	if (grid)
		flags = flags | Im2DViewportFlags_Grid;
	if (independentZoom)
		flags = flags | Im2DViewportFlags_AllowNonUniformZoom;
	ImGui::Checkbox("grid", &grid);
	ImGui::Checkbox("NouniformZoom", &independentZoom);

	Im2D::ViewerBegin("viewer1", ImVec2(0,0), flags);
	Im2D::ViewerEnd();

	ImGui::End();

	ImGui::Begin("multiple viewer with size");
	Im2D::ViewerBegin("##viewer1", ImVec2(200,200));
	Im2D::ViewerEnd();
	Im2D::ViewerBegin("viewer2", ImVec2(200, 200));
	Im2D::ViewerEnd();
	ImGui::End();
}