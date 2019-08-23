#pragma once
#include "../Im2D/Im2D.h"
#include "../Im2D/im2d_draw.h"
#include "../Paper/Path.h"
namespace Im2D {
	bool DragSegment(char * str_id, Segment * segment, std::string type = "BezierCorner") {
		bool changed{ false };

		// get points
		glm::vec2 handleIn = segment->_point + segment->_handleIn;
		glm::vec2 handleOut = segment->_point + segment->_handleOut;
		glm::vec2 P = segment->_point;

		// drag points
		ImGui::PushID(str_id);
		if (Im2D::DragPoint("##P", &P)) {
			segment->_point = P;
			changed = true;
		}
		if (!(handleIn.x == 0 && handleIn.y == 0) && Im2D::DragPoint("##in", &handleIn, 5.0)) {
			segment->_handleIn = handleIn - P;
			if (type == "Smooth") {
				segment->_handleOut = -(handleIn - P);
			}
			changed = true;
		}
		if (!(handleOut.x == 0 && handleOut.y == 0) && Im2D::DragPoint("##out", &handleOut, 5.0)) {
			segment->_handleOut = handleOut - P;
			if (type == "Smooth") {
				segment->_handleIn = -(handleOut - P);
			}
			
			changed = true;
		}
		addLineSegment(P, handleIn);
		addLineSegment(P, handleOut);
		ImGui::PopID();

		return changed;
	}
}