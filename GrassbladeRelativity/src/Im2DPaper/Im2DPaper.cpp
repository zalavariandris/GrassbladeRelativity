
#include "Im2DPaper.h"
#include "../Im2D/Im2D.h"
#include "../Im2D/im2d_draw.h"
void addPath(Paper::Path const & path) {
	for (auto & curve : path.getCurves()) {
		glm::vec2 A = curve.segment1()->point();
		glm::vec2 D = curve.segment2()->point();
		glm::vec2 B = A + curve.segment1()->handleOut();
		glm::vec2 C = D + curve.segment2()->handleIn();
		addBezierCurve(A, B, C, D);
	}
}

namespace Im2D {
	bool DragPath(char * str_id, Paper::Path * path) {

		ImGui::PushID(str_id);
		bool changed{ false };

		addPath(*path);

		auto & segments = path->getSegments();
		for (auto i = 0; i < segments.size(); i++) {
			auto & segment = segments[i];
			ImGui::PushID(&segment);

			glm::vec2 point = segment.point();
			if (Im2D::DragPoint("P", &point, 8)) {
				segment.point(point);
				changed = true;
			}

			if (i != 0) {
				glm::vec2 in = point + segment.handleIn();
				if (Im2D::DragPoint("in", &in, 4)) {
					segment.handleIn(in - point);
					changed = true;
				}
				addLineSegment(point, in, ImColor(255, 255, 255, 128));
			}

			if (i != segments.size() - 1) {
				glm::vec2 out = point + segment.handleOut();
				if (Im2D::DragPoint("out", &out, 4)) {
					segment.handleOut(out - point);
					changed = true;
				}
				addLineSegment(point, out, ImColor(255, 255, 255, 128));
			}

			ImGui::PopID();
		}
		ImGui::PopID();
		return changed;
	}
}