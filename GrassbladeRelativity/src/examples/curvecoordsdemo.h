#pragma once

#include "../Project.h"
#include <imgui.h>
#include "../Im2D/Im2D.h"
#include "../Im2D/im2d_draw.h"

#include "../Paper/Path.h"
#include "../Geo/Polygon.h"

#include "../Im2DPaper/Im2DPaper.h"
#include <vector>

#include "../utilities.h"

void showPolygon(Geo::Polygon poly,ImColor color=ImColor(255,255,255), bool showVertices=true) {
	addPolyline(poly.getVertices().data(), poly.getVertices().size(), color);
	for (auto P : poly.getVertices()) { addPoint(P, ImColor(0, 255, 255), 3); };
}

class Curvecoordsdemo : public Project {
public:
	void tick() {
		static Paper::Path sourcePath({ 
			Paper::Segment({0,-500},{200, 200}),
			Paper::Segment({0,500}, {100, 50})
			});
		static Paper::Path targetPath(sourcePath);

		ImGui::Begin("Inspector");
		if (ImGui::Button("copy from source")) {
			targetPath = sourcePath;
		}
		static int divideCount{ 100 };
		ImGui::DragInt("divide.count", &divideCount);
		ImGui::End();

		Geo::Polygon sourcePoly(divide(sourcePath, divideCount));
		Geo::Polygon targetPoly(divide(targetPath, divideCount));

		ImGui::Begin("Viewport");
		Im2D::BeginViewer("viewer");
		Im2D::DragPath("sourcePath", &sourcePath);
		Im2D::DragPath("targetPath", &sourcePath);
		showPolygon(sourcePoly, ImColor(0,0,255));
		showPolygon(targetPoly, ImColor(0,255,0));

		auto point = Im2D::GetMousePos();
		addPoint(point);
		auto derivedFromCurve = Field::pathToPath(sourcePath, targetPath, point);
		addArrow(point, derivedFromCurve);
		auto derivetFromPoly = Field::polyToPoly(sourcePoly, targetPoly, point);
		addArrow(point, derivetFromPoly, ImColor(0, 255,255));

		// show calculation
		double t = (double)sourcePoly.getNearestIndex(point) / sourcePoly.getVertices().size();
		auto closestVertex = sourcePoly.getPointAtTime(t);
		auto distance = glm::distance(point, closestVertex);

		auto targetVertex = targetPoly.getPointAtTime(t);
		auto targetTangent = targetPoly.getNormalAtIndex(t*targetPoly.getVertices().size());

		Im2D::EndViewer();
		ImGui::End();
	}
};