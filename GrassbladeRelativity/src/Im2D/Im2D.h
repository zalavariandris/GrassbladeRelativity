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

class Rect {
private:
	double _x;
	double _y;
	double _w;
	double _h;
public:
	Rect() {};
	Rect(glm::vec2 point, double width, double height) :
		_x(point.x), _y(point.y), _w(width), _h(height) {
	
	};

	Rect(glm::vec2 from, glm::vec2 to) {
		_x = from.x;
		_y = from.y;
		_w = to.x - from.x;
		_h = to.y - from.y;
		if (_w < 0) {
			_x = to.x;
			_w = -_w;
		}
		if (_h < 0) {
			_y = to.y;
			_h = -_h;
		}
	};

	double left() {
		return _x;
	}
	
	double right() {
		return _x + _w;
	}
	double top() {
		return _y;
	}

	double bottom() {
		return _y + _h;
	}

	bool contains(glm::vec2 point) {
		return point.x > left()
			&& point.x < right()
			&& point.y > top()
			&&point.y < bottom();
	}
};

struct Im2DContext {
	glm::mat3 viewMatrix;
	glm::mat3 projectionMatrix;
	glm::vec2 viewportPos;
	glm::vec2 viewportSize;
};

namespace Im2D {
	Im2DContext * GetCurrentContext();

	void ViewerBegin(const char* label_id, const ImVec2 & size = ImVec2(0, 0), Im2DViewportFlags flags= Im2DViewportFlags_Grid);

	void ViewerEnd();

	glm::vec2 getZoom();
	glm::vec2 getPan();

	// get mouse pos in viewport coordinates
	glm::vec2 GetMousePos();

	// get mouse delta respect to viewport scale
	glm::vec2 GetMouseDelta();

	// Gizmos
	bool InvisibleButton(const char * label_id, glm::vec2 pos, glm::vec2 size);
	bool Button(const char * label_id, glm::vec2 pos, glm::vec2 size);
	bool DragPoint(const char * label_id, glm::vec2 * P, float r = 8);
	bool DragBezierSegment(const char * str_id, glm::vec2 * A, glm::vec2 * B, glm::vec2 * C, glm::vec2 * D);

	// Items
}
