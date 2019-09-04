#pragma once

#include "Im2D.h"
#include "im2d_draw.h"
#include "imgui_internal.h"
#include <map>
#include <cstring> //strstr, to find ## in label_id
#include <string>
#include <iostream>
#include "imconfig.h"
//#include "ofMain.h"

Im2DContext * Im2D::GetCurrentContext() {
	static Im2DContext * ctx = new Im2DContext();
	return ctx;
}

glm::vec2 Im2D::GetMousePos() {
	ImVec2 mousePos = ImGui::GetMousePos();
	return fromScreen(glm::vec2( mousePos.x, mousePos.y ));
}

glm::vec2 Im2D::GetMouseDelta() {
	Im2DContext * ctx = Im2D::GetCurrentContext();

	ImGuiIO io = ImGui::GetIO();
		
	glm::vec2 scale = Im2D::getZoom();
	return glm::vec2(
		io.MouseDelta.x / scale.x,
		io.MouseDelta.y / scale.y
	);
}

bool Im2D::DragBezierSegment(const char * str_id, glm::vec2 * A, glm::vec2 * B, glm::vec2 * C, glm::vec2 * D) {
	// draw curve
	addBezierSegment(*A, *B, *C, *D);
	
	// draw tangents
	addLineSegment(*A, *B);
	addLineSegment(*C, *D);

	bool changed{ false };

	ImGui::PushID(str_id);

	if (Im2D::DragPoint("A", A))
		changed = true;

	if (Im2D::DragPoint("B", B))
		changed = true;

	if (Im2D::DragPoint("C", C))
		changed = true;

	if (Im2D::DragPoint("D", D))
		changed = true;

	ImGui::PopID();
	
	return changed;
}

// Viewer
bool ScreenControls(const char * str_id, const ImVec2 & size, glm::mat3 * viewMatrix, bool IndependentZoom=false) {
	bool changed{ false };
	// ImGui does not allow zero size Invisible Button.
	if (size.x != 0.0f && size.y != 0.0f) {
		ImGui::InvisibleButton(str_id, size);
		ImGui::SetItemAllowOverlap();
	}
	auto io = ImGui::GetIO();

	if (ImGui::IsItemHovered(ImGuiHoveredFlags_None) && ImGui::IsMouseDown(2)) {
		ImGui::ActivateItem(ImGui::GetItemID());
	}
	if (ImGui::IsItemActive() && ImGui::IsMouseDragging(2)) {
		ImGui::ActivateItem(ImGui::GetItemID());
	}

	glm::vec2 zoom = glm::vec2( (*viewMatrix)[0][0], (*viewMatrix)[1][1] ); // get zoom
	glm::vec2 pan((*viewMatrix)[2][0] / zoom.x, (*viewMatrix)[2][1] / zoom.y); //get pan

	if (ImGui::IsItemActive() && (ImGui::IsMouseDragging(2) && !ImGui::GetIO().KeyAlt ||
		(ImGui::IsMouseDragging(0) && ImGui::GetIO().KeyAlt && !ImGui::GetIO().KeyCtrl))) {
		glm::vec2 scale = glm::vec2((*viewMatrix)[0][0], (*viewMatrix)[1][1]);
		float h = io.MouseDelta.x * 1 / scale.x; // horizontal
		float v = io.MouseDelta.y * 1 / scale.y; // vertical

		pan += glm::vec2(h, v);
		changed = true;
	}

	// handle zoom with mouse wheel
	if (io.MouseWheel != 0 && ImGui::IsItemHovered(ImGuiHoveredFlags_None)) {
		zoom *= 1 + io.MouseWheel*0.1;
		changed = true;
	}

	// handle zoom with MMB+alt
	if (ImGui::IsItemActive() && // item is active
		((ImGui::IsMouseDragging(2) && ImGui::GetIO().KeyAlt) || // MMB and ALT
		(ImGui::IsMouseDragging(0) && ImGui::GetIO().KeyAlt && ImGui::GetIO().KeyCtrl))) //LMB + CTRL+ALT
	{ 
		glm::vec2 scale = glm::vec2((*viewMatrix)[0][0], (*viewMatrix)[1][1]);
		float zoomSpeed = 0.01;
		float horizontalZoomFactor = io.MouseDelta.x * 1 * zoomSpeed; // horizontal
		float verticalZoomFactor = io.MouseDelta.y * -1 * zoomSpeed; // vertical
		if (!IndependentZoom) {
			horizontalZoomFactor = verticalZoomFactor = (horizontalZoomFactor + verticalZoomFactor) / 2;
		}
		zoom *= glm::vec2(1 + horizontalZoomFactor, 1 + verticalZoomFactor);
		changed = true;
	}

	if (changed) {
		*viewMatrix = glm::mat3({       zoom.x,          0, 0,
						                   0,       zoom.y, 0,
						          pan.x*zoom.x, pan.y*zoom.y, 1 });
	}

	return changed;
}

void Im2D::ViewerBegin(const char* label_id, const ImVec2 & size, Im2DViewportFlags flags) {
	assert(GetCurrentContext()->CurrentViewer == nullptr); /* missing end of the last viewer*/

	ImGui::BeginChild(label_id, size, true,
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse);

	//
	ImGuiWindow * window = ImGui::GetCurrentWindow();
	ImGuiID id = window->GetID(label_id);
	ImRect contentRect = window->ContentsRegionRect; // get the child window content rect in __screen space__
	ImDrawList * drawList = window->DrawList;

	//ImGuiContext * g = ImGui::GetCurrentContext();
	//const int current_frame = g->FrameCount;
	//const bool first_begin_of_the_frame = (window->LastFrameActive != current_frame);
	//std::cout << "id:" << window->GetID(label_id) << "first_begin: " << first_begin_of_the_frame << std::endl;

	// keep a seperate state for each viewer
	static std::map<ImGuiID, Im2DViewer*> viewers;
	if (viewers.find(id) == viewers.end()) {
		viewers[id] = new Im2DViewer();
	}
	Im2DViewer * viewer = viewers.at(id);

	// Update Im2D context to match the actual viewer state
	Im2DContext * ctx = GetCurrentContext();
	ctx->CurrentViewer = viewer;

	// 
	const int current_frame = ImGui::GetCurrentContext()->FrameCount;
	const bool first_begin_of_the_frame = (viewer->LastFrameActive != current_frame);
	if (first_begin_of_the_frame) {
		viewer->LastFrameActive = current_frame;
	}

	// set projection matrix to the viewer's content rectangle
	viewer->projectionMatrix = glm::mat3(
		1,     0,     0,
		0,     1,     0,
		contentRect.GetCenter().x , contentRect.GetCenter().y, 1
	);

	// When reusing viewport again multiple times a frame, just append content (don't need to setup again)
	if (first_begin_of_the_frame) {
		// Control screen
		ScreenControls("ViewerControls", contentRect.GetSize(), &(viewer->viewMatrix), flags & Im2DViewportFlags_AllowNonUniformZoom);

		/* Render */
		// add label at the top left corner
		drawList->AddText(contentRect.Min, ImColor(255, 255, 255), label_id);

		// add adaptive grid
		if (flags & Im2DViewportFlags_Grid)
			addAdaptiveGrid();
	}
}

void Im2D::ViewerEnd() {
	Im2DContext * ctx = GetCurrentContext();
	ctx->CurrentViewer = nullptr;
	ImGui::EndChild();
}

glm::vec2 Im2D::getZoom() {
	Im2DContext * ctx = GetCurrentContext();
	glm::mat3 viewMatrix = ctx->CurrentViewer->viewMatrix;
	return glm::vec2(viewMatrix[0][0], viewMatrix[1][1]);
}

glm::vec2 Im2D::getPan() {
	Im2DContext * ctx = GetCurrentContext();
	glm::mat3 viewMatrix = ctx->CurrentViewer->viewMatrix;
	float zoom = viewMatrix[0][0];
	return glm::vec2(viewMatrix[2][0] / zoom, viewMatrix[2][1] / zoom);
}

// Gizmos
bool Im2D::InvisibleButton(const char * label_id, glm::vec2 center, double width, double height) {
	ImGuiWindow * window = ImGui::GetCurrentWindow();
	Im2DContext * ctx = Im2D::GetCurrentContext();
	glm::vec2 P1 = toScreen(center);
	ImGui::SetCursorScreenPos(ImVec2(P1.x-width/2, P1.y-height/2));
	return ImGui::InvisibleButton(label_id, ImVec2(width, height));
}

bool Im2D::Button(const char * label_id, glm::vec2 center, double width, double height) {
	ImGuiWindow * window = ImGui::GetCurrentWindow();
	Im2DContext * ctx = Im2D::GetCurrentContext();
	glm::vec2 P1 = toScreen(center);
	ImGui::SetCursorScreenPos(ImVec2(P1.x - width / 2, P1.y - height / 2));
	return ImGui::Button(label_id, ImVec2(width, height));
}

bool Im2D::DragPoint(const char * label_id, glm::vec2 * P, float radius) {
	Im2DContext * ctx = Im2D::GetCurrentContext();
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, radius);
	Im2D::Button(label_id, *P, radius*2, radius*2);
	ImGui::PopStyleVar();

	bool changed{ false };
	if (ImGui::IsItemActive() && ImGui::IsMouseDragging()) {
		ImGuiIO io = ImGui::GetIO();
		glm::vec2 scale(ctx->CurrentViewer->viewMatrix[0][0], ctx->CurrentViewer->viewMatrix[1][1]);
		P->x += io.MouseDelta.x / scale.x;
		P->y += io.MouseDelta.y / scale.y;
		changed = true;
	}

	// add label
	char * fmt = ImGui::IsItemHovered() || ImGui::IsItemActive() ? "(%.2f, %.2f)" : "";
	glm::vec2 offset = glm::vec2(radius, -radius) + glm::vec2(0, -ImGui::GetTextLineHeight());
	float scale = ctx->CurrentViewer->viewMatrix[0][0];
	offset *= 1 / scale;
	addText(*P+offset, fmt, P->x, P->y);

	return changed;
}
