#pragma once
#include "../Paper/Path.h"

void addPath(Paper::Path const & path);

namespace Im2D {
	bool DragPath(char * str_id, Paper::Path * path);
}