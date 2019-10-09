#pragma once
#include "imgui.h"
#include "glm/glm.hpp"

typedef int Im2DViewportFlags;
enum Im2DViewportFlags_
{
	Im2DViewportFlags_None = 0,
	Im2DViewportFlags_Grid = 1 << 0,  // Draw an adpative grid in the background
	Im2DViewportFlags_AllowNonUniformZoom = 1 << 1   // allow zooming x and y axis independently
};

struct Im2DViewer {
	glm::mat3 viewMatrix{ 1 };
	glm::mat3 projectionMatrix{ 1 };
	int LastFrameActive = -1;
};

struct Im2DContext {
	Im2DViewer * CurrentViewer;
};

namespace Im2D {
	Im2DContext * GetCurrentContext();

	void BeginViewer(const char* label_id, const ImVec2 & size = ImVec2(0, 0), Im2DViewportFlags flags= Im2DViewportFlags_Grid);

	void EndViewer();

	glm::vec2 getZoom();
	glm::vec2 getPan();

	// get mouse pos in viewport coordinates
	glm::vec2 GetMousePos();

	// get mouse delta respect to viewport scale
	glm::vec2 GetMouseDelta();

	// Gizmos
	bool InvisibleButton(const char * label_id, glm::vec2 pos, double width, double height);
	bool Button(const char * label_id, glm::vec2 pos, double width, double height);
	bool DragPoint(const char * label_id, glm::vec2 * P, float radius = 8);
	bool DragBezierSegment(const char * str_id, glm::vec2 * A, glm::vec2 * B, glm::vec2 * C, glm::vec2 * D);
	void Image(glm::vec2 center, ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0));
	// Items

}



