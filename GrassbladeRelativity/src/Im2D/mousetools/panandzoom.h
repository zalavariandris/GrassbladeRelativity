#pragma once
#include "../Im2D.h"
#include <glm/glm.hpp>

namespace Im2D {
	bool PanAndZoom(const char * str_id, glm::mat3 * viewMatrix, bool IndependentZoom = false);
}