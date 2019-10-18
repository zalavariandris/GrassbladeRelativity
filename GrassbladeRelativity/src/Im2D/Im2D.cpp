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
#include "mousetools/panandzoom.h"
#include "mousetools/selectandmove.h"
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

glm::vec2 Im2D::GetMouseDragDelta() {
	return glm::vec2(ImGui::GetMouseDragDelta()) / Im2D::getZoom();
}

bool Im2D::DragBezierSegment(const char * str_id, glm::vec2 * A, glm::vec2 * B, glm::vec2 * C, glm::vec2 * D) {
	// draw curve
	addBezierCurve(*A, *B, *C, *D);
	
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
void Im2D::BeginViewer(const char* label_id, const ImVec2 & size, Im2DViewportFlags flags) {
	assert(GetCurrentContext()->CurrentViewer == nullptr); /* missing end of the last viewer*/

	ImGui::BeginChild(label_id, size, true,
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse);

	//
	ImGuiWindow * window = ImGui::GetCurrentWindow();
	ImGuiID id = window->GetID(label_id);
	ImRect workRect = window->WorkRect;
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


	// set projection matrix to the viewer's content rectangle
	viewer->projectionMatrix = glm::mat3(
		1,     0,     0,
		0,     1,     0,
		workRect.GetCenter().x , workRect.GetCenter().y, 1
	);

	// When reusing viewport again multiple times a frame, just append content (don't need to setup again)
	if (first_begin_of_the_frame) {
		// Control screen
		PanAndZoom("ViewerControls", &(viewer->viewMatrix), flags & Im2DViewportFlags_AllowNonUniformZoom);
		//SelectAndMoveItems();
		/* Render */
		// add label at the top left corner
		drawList->AddText(workRect.Min, ImColor(255, 255, 255), label_id);

		// add adaptive grid
		if (flags & Im2DViewportFlags_Grid)
			addAdaptiveGrid();
	}
}

void Im2D::EndViewer() {
	Im2DContext * ctx = GetCurrentContext();
	const int current_frame = ImGui::GetCurrentContext()->FrameCount;

	auto viewer = GetCurrentContext()->CurrentViewer;
	const bool first_begin_of_the_frame = (viewer->LastFrameActive != current_frame);
	if (first_begin_of_the_frame) {
		viewer->LastFrameActive = current_frame;
	}
	if (first_begin_of_the_frame) {
		SelectAndMoveItems();
	}
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

void Im2D::Image(glm::vec2 pos, ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col) {
	ImGuiWindow * window = ImGui::GetCurrentWindow();
	Im2DContext * ctx = Im2D::GetCurrentContext();
	auto topLeft = pos;
	auto bottomRight = glm::vec2(size.x, size.y);
	auto screenPos = toScreen(topLeft);
	auto screenSize = toScreen(bottomRight)- screenPos;
	ImGui::SetCursorScreenPos(ImVec2(screenPos.x, screenPos.y));
	ImGui::Image(user_texture_id, ImVec2(screenSize.x, screenSize.y), uv0, uv1, tint_col, border_col);
}

void Im2D::Item(const char * label_id, glm::vec2 * pos, bool * selected) {
	// create items
	auto item = Im2DItem(pos, selected);

	// push to items list
	auto ctx = Im2D::GetCurrentContext();
	ctx->items.push_back(Im2DItem(pos, selected));

	// display item
	addPoint(*item.pos, *item.selected ? ImColor(255, 255, 255) : ImColor(128, 128, 128), 5.0f);
}