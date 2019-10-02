#pragma once
#include <glm/glm.hpp>
#include "../numerical.h"
namespace Paper {
	namespace Point {
		bool isZero(glm::vec2 const & P);

		bool isCollinear(glm::vec2 const & A, glm::vec2 const & B);

		bool isClose(glm::vec2 const & P, glm::vec2 const & Q, double epsilon = Numerical::EPSILON);
	}
}