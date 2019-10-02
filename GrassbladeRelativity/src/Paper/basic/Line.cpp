#include "Line.h"
#include "Point.h"
namespace Paper {
	double Line::getDistance(glm::vec2 const & P) const {
		return abs(getSignedDistance(px, py, vx, vy, P.x, P.y));
	}

	double Line::getSignedDistance(double px, double py, double vx, double vy, double x, double y, bool asVector){
		if (!asVector) {
			vx -= px;
			vy -= py;
		}

		// Based on the error analysis by @iconexperience outlined in #799
		return  vx == 0 ? (vy > 0 ? x - px : px - x)
			: vy == 0 ? (vx < 0 ? y - py : py - y)
			: ((x - px) * vy - (y - py) * vx) / (
				vy > vx
				? vy * sqrt(1 + (vx * vx) / (vy * vy))
				: vx * sqrt(1 + (vy * vy) / (vx * vx))
				);
	};

	bool Line::isCollinear(Line const & line) const{
		return Point::isCollinear(glm::vec2(vx,vy), glm::vec2(line.vx, line.vy));
	}
}