#include "panandzoom.h"
#include "imgui_internal.h"

bool Im2D::PanAndZoom(const char * str_id, glm::mat3 * viewMatrix, bool IndependentZoom) {
	bool changed{ false };

	// mouse state shortcuts
	auto window = ImGui::GetCurrentWindow();
	// by default window docus on left click only.
	// focus window on middle click!
	if (ImGui::IsMouseClicked(2) && ImGui::IsWindowHovered()) {
		ImGui::FocusWindow(window);
	}
	//bool isMousePressed = ImGui::IsMouseClicked(0) && ImGui::GetIO().KeyAlt && ImGui::IsWindowHovered();
	bool isAltDragging = ImGui::IsMouseDragging(0) && ImGui::GetIO().KeyAlt && ImGui::IsWindowFocused();
	bool isMouseReleased = ImGui::IsMouseReleased(0) && ImGui::GetIO().KeyAlt && ImGui::IsWindowFocused();
	bool isMiddleDragging = ImGui::IsMouseDragging(2) && !ImGui::GetIO().KeyAlt && ImGui::IsWindowFocused();

	glm::vec2 zoom = Im2D::getZoom();
	glm::vec2 pan = Im2D::getPan();

	// pan with middle mouse drag
	if (isMiddleDragging) {
		pan += Im2D::GetMouseDelta();
		changed = true;
	}

	// pan with alt drag
	if (isAltDragging && !ImGui::GetIO().KeyCtrl) {
		pan += Im2D::GetMouseDelta();
		changed = true;
	}

	// handle zoom with mouse wheel
	auto io = ImGui::GetIO();
	if (io.MouseWheel != 0 && ImGui::IsItemHovered(ImGuiHoveredFlags_None)) {
		zoom *= 1 + io.MouseWheel*0.1;
		changed = true;
	}

	// handle zoom with MMB+alt
	if (ImGui::IsWindowFocused() && // item is active
		((ImGui::IsMouseDragging(2) && ImGui::GetIO().KeyAlt) || // MMB and ALT
		(ImGui::IsMouseDragging(0) && ImGui::GetIO().KeyAlt && ImGui::GetIO().KeyCtrl))) //LMB + CTRL+ALT
	{
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
		auto ctx = Im2D::GetCurrentContext();
		ctx->CurrentViewer->viewMatrix = glm::mat3({
			zoom.x,            0,       0,				  
			0,            zoom.y,       0,
			pan.x*zoom.x, pan.y*zoom.y, 1 
		});
	}

	return changed;
}