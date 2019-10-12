#pragma once
#include "../Im2D/Im2D.h"
#include "../Im2D/im2d_draw.h"
#include "../Paper/Path.h"

void addPath(Paper::Path const & path){
	for (auto & curve : path.getCurves()) {
		glm::vec2 A = curve->_segment1->point();
		glm::vec2 D = curve->_segment2->point();
		glm::vec2 B = A + curve->_segment1->handleOut();
		glm::vec2 C = D + curve->_segment2->handleIn();
		addBezierCurve(A, B, C, D);
	}
}

namespace Im2D {
	//bool DragSegment(char * str_id, Paper::Segment * segment, std::string type = "BezierCorner") {
	//	bool changed{ false };

	//	// get points
	//	glm::vec2 handleIn = segment->point() + segment->handleIn();
	//	glm::vec2 handleOut = segment->point() + segment->handleOut();
	//	glm::vec2 P = segment->point();

	//	// drag points
	//	ImGui::PushID(str_id);
	//	if (Im2D::DragPoint("##P", &P)) {
	//		segment->point() = P;
	//		changed = true;
	//	}
	//	if (!(handleIn.x == 0 && handleIn.y == 0) && Im2D::DragPoint("##in", &handleIn, 5.0)) {
	//		segment->handleIn(handleIn - P);
	//		changed = true;
	//	}
	//	if (!(handleOut.x == 0 && handleOut.y == 0) && Im2D::DragPoint("##out", &handleOut, 5.0)) {
	//		segment->handleOut(handleOut - P);			
	//		changed = true;
	//	}
	//	addLineSegment(P, handleIn);
	//	addLineSegment(P, handleOut);
	//	ImGui::PopID();

	//	return changed;
	//}

	bool DragPath(char * str_id, Paper::Path * path) {

		ImGui::PushID(str_id);
		bool changed{ false };

		addPath(*path);

		auto & segments = path->getSegments();
		for (auto i = 0; i < segments.size(); i++) {
			auto & segment = segments[i];
			ImGui::PushID(segment.get());

			glm::vec2 point = segment->point();
			if (Im2D::DragPoint("point", &point)) {
				segment->point(point);
				changed = true;
			}

			if (i != 0) {
				glm::vec2 in = point + segment->handleIn();
				if (Im2D::DragPoint("in", &in)) {
					cout << "set handle in" << endl;
					segment->handleIn(in - point);
					changed = true;
				}
				addLineSegment(point, in);
			}

			if (i != segments.size()-1) {
				glm::vec2 out = point + segment->handleOut();
				if (Im2D::DragPoint("out", &out)) {
					cout << "set handle out" << endl;
					segment->handleOut(out - point);
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