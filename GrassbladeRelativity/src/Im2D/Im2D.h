#pragma once
#include "imgui.h"
#include "glm/glm.hpp"

enum Im2DViewportFlags_
{
	Im2DViewportFlags_None = 0,
	Im2DViewportFlags_Grid = 1 << 0,  // Draw an adpative grid in the background
	Im2DViewportFlags_AllowNonUniformZoom = 1 << 1   // allow zooming x and y axis independently
};

struct Im2DContext {
	glm::mat3 viewMatrix;
	glm::mat3 projectionMatrix;
	glm::vec2 viewportPos;
	glm::vec2 viewportSize;
};

namespace Im2D {
	Im2DContext * GetCurrentContext();

	void ViewerBegin(const char* label_id, const ImVec2 & size = ImVec2(0, 0), Im2DViewportFlags_ flags= Im2DViewportFlags_Grid);

	void ViewerEnd();

	float getZoom();
	glm::vec2 getPan();

	glm::vec2 GetMousePos();

	// Gizmos
	bool DragPoint(char * label_id, glm::vec2 * P, float r = 8);
	bool DragBezierSegment(char * str_id, glm::vec2 * A, glm::vec2 * B, glm::vec2 * C, glm::vec2 * D);
}
