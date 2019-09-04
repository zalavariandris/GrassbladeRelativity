#pragma once
#include "../Animation/AnimCurve.h"

/* Widgets */
namespace Widgets
{
	void TimeSlider(const char * label_id, int * frame, bool * play, int * begin, int * end);

	void GraphEditor(const char * label_id, std::vector<Animation::AnimCurve*> curves, int * F);
}
