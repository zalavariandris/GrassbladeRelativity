#pragma once

#include "Im2D.h"
#include "im2d_draw.h"
#include "imgui_internal.h"
#include <map>
//#include "ofMain.h"

Im2DContext * Im2D::GetCurrentContext() {
	static Im2DContext * ctx = new Im2DContext();
	return ctx;
}

glm::vec2 Im2D::GetMousePos() {
	ImVec2 mousePos = ImGui::GetMousePos();
	return fromScreen(glm::vec2( mousePos.x, mousePos.y ));
}

bool Im2D::DragBezierSegment(char * str_id, glm::vec2 * A, glm::vec2 * B, glm::vec2 * C, glm::vec2 * D) {
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
bool ScreenControls(char * str_id, const ImVec2 & size, glm::mat3 * viewMatrix) {
	bool changed{ false };
	ImGui::InvisibleButton(str_id, size);
	ImGui::SetItemAllowOverlap();
	ImGuiIO io = ImGui::GetIO();

	if (ImGui::IsItemHovered(ImGuiHoveredFlags_None) && ImGui::IsMouseDown(2)) {
		ImGui::ActivateItem(ImGui::GetItemID());
	}
	if (ImGui::IsItemActive() && ImGui::IsMouseDragging(2)) {
		ImGui::ActivateItem(ImGui::GetItemID());
	}

	float zoom = (*viewMatrix)[0][0];
	glm::vec2 pan((*viewMatrix)[2][0] / zoom, (*viewMatrix)[2][1] / zoom);

	if (ImGui::IsItemActive() && (ImGui::IsMouseDragging(2) ||
		(ImGui::IsMouseDragging(0) && ImGui::GetIO().KeyAlt))) {
		float scale = (*viewMatrix)[0][0];
		float h = io.MouseDelta.x * 1 / scale; // horizontal
		float v = io.MouseDelta.y * 1 / scale; // vertical

		pan += glm::vec2(h, v);
		changed = true;
	}

	// handle scale with mouse wheel
	if (io.MouseWheel != 0 && ImGui::IsItemHovered(ImGuiHoveredFlags_None)) {
		zoom *= 1 + io.MouseWheel*0.1;

		changed = true;
	}

	if (changed) {
		*viewMatrix = glm::mat3({ zoom,0,0,
						0,zoom,0,
						pan.x*zoom, pan.y*zoom, 1 });
	}

	return changed;
}

void Im2D::ViewerBegin(const char* label_id, const ImVec2 & size) {
	ImGui::BeginChild(label_id, size, true,
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse);

	ImGuiWindow * window = ImGui::GetCurrentWindow();
	ImGuiID id = window->GetID(label_id);

	static std::map<ImGuiID, glm::mat3> storage_mat;
	glm::mat3 viewMatrix = storage_mat.count(id) > 0 ? storage_mat[id] : glm::mat3();
	ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
	ImVec2 windowSize = ImGui::GetWindowSize();
	glm::mat3 projectionMatrix{
		1,     0,     0,
		0,     1,     0,
		(cursorScreenPos.x + cursorScreenPos.x+windowSize.x) / 2, (cursorScreenPos.y + cursorScreenPos.y + windowSize.y) / 2, 1
	};

	// Screen control
	ImGui::SetCursorScreenPos(cursorScreenPos);
	if (ScreenControls("ViewerControls", windowSize, &viewMatrix)) {
		storage_mat[id] = viewMatrix;
	}

	// Update Im2D context
	Im2DContext * ctx = GetCurrentContext();
	ctx->viewMatrix = viewMatrix;
	ctx->projectionMatrix = projectionMatrix;

	// Render
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	// Toolbar
	ImGui::SetCursorScreenPos(cursorScreenPos);
	ImGui::Text(label_id);

	// Add adaptive grid
	addGrid();
}

void Im2D::ViewerEnd() {
	Im2DContext * ctx = GetCurrentContext();
	ctx->viewMatrix = glm::mat3(1);
	ctx->viewMatrix = glm::mat3(1);
	ImGui::EndChild();
}

// Gizmos
bool Im2D::DragPoint(char * label_id, glm::vec2 * P, float r) {
	ImGuiWindow * window = ImGui::GetCurrentWindow();
	Im2DContext * ctx = Im2D::GetCurrentContext();
	glm::vec2 P1 = toScreen(*P);
	ImGui::SetCursorScreenPos(ImVec2(P1.x-r, P1.y-r));
	ImGui::InvisibleButton(label_id, ImVec2(r * 2, r * 2));

	bool changed{ false };
	if (ImGui::IsItemActive() && ImGui::IsMouseDragging()) {
		ImGuiIO io = ImGui::GetIO();
		glm::vec2 scale(ctx->viewMatrix[0][0], ctx->viewMatrix[1][1]);
		P->x += io.MouseDelta.x / scale.x;
		P->y += io.MouseDelta.y / scale.y;
		changed = true;
	}

	ImColor color = ImGui::IsItemHovered() || ImGui::IsItemActive() ? ImColor(255, 255, 255) : ImColor(128, 128, 128);
	addPoint(*P, color, r);
	char * fmt = ImGui::IsItemHovered() || ImGui::IsItemActive() ? "%s (%.2f, %.2f)" : "%s";

	glm::vec2 offset = glm::vec2(r, -r) + glm::vec2(0, -ImGui::GetTextLineHeight());
	float scale = ctx->viewMatrix[0][0];
	offset *= 1 / scale;
	addText(*P+offset, fmt, label_id, P->x, P->y);

	return changed;
}
