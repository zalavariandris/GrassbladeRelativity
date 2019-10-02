#pragma once
#include <glm/glm.hpp>
#include "../numerical.h"
namespace Paper {
	namespace Point {
		bool isZero(glm::vec2 const & P) {
			return Numerical::isZero(P.x) && Numerical::isZero(P.y);
		}

		bool isCollinear(glm::vec2 const & A, glm::vec2 const & B) {
			// NOTE: We use normalized vectors so that the epsilon comparison is
			// reliable. We could instead scale the epsilon based on the vector
			// length. But instead of normalizing the vectors before calculating
			// the cross product, we can scale the epsilon accordingly.
			return abs(A.x * B.y - A.y * B.x)
				<= sqrt((A.x * A.x + A.y * A.y) * (B.x * B.x + B.y * B.y))
				* /*#=*/Numerical::TRIGONOMETRIC_EPSILON;
		}

		bool isClose(glm::vec2 const & P, glm::vec2 const & Q, double epsilon = Numerical::EPSILON) {
			
			return glm::distance(P, Q) < epsilon;
		}
	}
}