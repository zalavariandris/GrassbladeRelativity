#pragma once

#include <glm/glm.hpp>

namespace Paper {
	class Line
	{
	private:
		double px, py, vx, vy;
	public:
		Line(double px, double py, double vx, double vy) : px(px), py(py), vx(vx), vy(vy) {};
		Line(glm::vec2 P, glm::vec2 V) : px(P.x), py(P.y), vx(V.x), vy(V.y) {};

		double getDistance(glm::vec2 const & P) const;

		bool isCollinear(Line const & line) const;

		static double getSignedDistance(double px, double py, double vx, double vy, double x, double y, bool asVector = false);
	};
}

