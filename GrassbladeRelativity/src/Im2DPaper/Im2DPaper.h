#pragma once
#include "../Im2D/Im2D.h"
#include "../Im2D/im2d_draw.h"
#include "../Paper/Path.h"
namespace Im2D {
	bool DragSegment(char * str_id, Paper::Segment * segment, std::string type = "BezierCorner") {
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

	bool DragPath(char * str_id, Paper::Path * path) {

		ImGui::PushID(str_id);
		bool changed{ false };

		for (auto & curve : path->getCurves()) {
			glm::vec2 A = curve->_segment1->_point;
			glm::vec2 D = curve->_segment2->_point;
			glm::vec2 B = A + curve->_segment1->_handleOut;
			glm::vec2 C = D + curve->_segment2->_handleIn;
			addBezierCurve(A, B, C, D);
		}

		auto & segments = path->getSegments();
		for (auto i = 0; i < segments.size(); i++) {
			auto & segment = segments[i];
			ImGui::PushID(segment.get());

			glm::vec2 point = segment->_point;
			if (Im2D::DragPoint("point", &point)) {
				segment->_point = point;
				changed = true;
			}

			if (i != 0) {
				glm::vec2 in = point + segment->_handleIn;
				if (Im2D::DragPoint("in", &in)) {
					segment->_handleIn = in - point;
					changed = true;
				}
				addLineSegment(point, in);
			}

			if (i != segments.size()-1) {
				glm::vec2 out = point + segment->_handleOut;
				if (Im2D::DragPoint("out", &out)) {
					segment->_handleOut = out - point;
					changed = true;
				}
				addLineSegment(point, out);
			}

			ImGui::PopID();
		}
		ImGui::PopID();
		return changed;
	}
}