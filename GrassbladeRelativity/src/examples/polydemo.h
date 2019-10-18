#pragma once
#include "../Project.h"
#include "../Geo/Polygon.h"
#include <imgui.h>
#include "../Im2D/Im2D.h"
#include "../Im2D/im2d_draw.h"


namespace Im2D {
	bool DragPolygon(char * str_id, Geo::Polygon * poly) {
		bool changed{ false };
		for (auto & point : poly->getVertices()) {
			ImGui::PushID(&point);
			if (Im2D::DragPoint("##P", &point)) {
				changed = true;
			}
			ImGui::PopID();
		}

		auto window = ImGui::GetCurrentWindow();
		addPolyline(poly->getPoints().data(), poly->getPoints().size());

		return changed;
	}
}

class Polydemo : public Project {
public:
	void tick() {
		static Geo::Polygon poly({ {0,0}, {100, 100}, {200, 0} });
		ImGui::Begin("Inspector");
		static float offset{ 0 };
		ImGui::DragFloat("pointAtOffset", &offset);
		auto point = poly.getPointAt(offset);
		
		ImGui::Text("length: %f", poly.getLength());
		ImGui::End();

		ImGui::Begin("Viewport");
		Im2D::BeginViewer("viewer");

		Im2D::DragPolygon("poly", &poly);
		addPoint(point);
		auto idx = poly.getNearestIndex(Im2D::GetMousePos());
		if (idx >= 0) {
			addPoint(poly.getPoints()[idx]);
		}

		for (auto i = 0; i < poly.getVertices().size(); i++) {
			auto P = poly.getVertices()[i];
			auto N = poly.getNormalAtIndex(i);
			addLineSegment(P, P + N*100, ImColor(0, 255,255));
		}

		Im2D::EndViewer();
		ImGui::End();
	}
};