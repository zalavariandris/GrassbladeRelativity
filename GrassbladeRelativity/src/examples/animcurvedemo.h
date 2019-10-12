#pragma once
#include "../Project.h"
#include "imgui.h"
#include "../Im2D/Im2D.h"
#include "../Im2D/im2d_draw.h"
#include "../Animation/AnimationCurve.h"
#include "imgui_internal.h"
#include "../Widgets/Grapheditor.hpp"
class AnimcurveDemo :Project {
public:
	AnimcurveDemo() {
		
	}

	~AnimcurveDemo() {

	}

	void tick() {
		static Animation::AnimationCurve curve({
			{0, 100, Animation::Keyframe::Spline},
			{250, 100, Animation::Keyframe::Spline},
			{500, -100, Animation::Keyframe::Spline }
			});

		ImGui::Begin("Grapheditor");
		Widgets::Grapheditor("grapheditor", { &curve }, 0);
		ImGui::End();
	}
};