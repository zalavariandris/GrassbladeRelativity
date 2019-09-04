#pragma once
#include "Widgets/Widgets.hpp"

void showGraphEditorDemo() {
	static Animation::AnimCurve animCurveX({
		Animation::Key(0,0),
		Animation::Key(50,50),
		Animation::Key(100,100),
		Animation::Key(150,150),
		Animation::Key(200,200),
		Animation::Key(250,250)
		});
	animCurveX.label = "x";

	static Animation::AnimCurve animCurveY({
		Animation::Key(0,250),
		Animation::Key(50,200),
		Animation::Key(100,50),
		Animation::Key(150,100),
		Animation::Key(200,50),
		Animation::Key(250,50)
		});
	animCurveY.label = "y";
	static int F{ 0 };

	ImGui::Begin("GraphEditor");
	ImGui::DragInt("F", &F);
	Widgets::GraphEditor("grapheditor", { &animCurveX, &animCurveY }, &F);
	ImGui::End();
}

void showTimeSliderDemo() {
	ImGui::Begin("TimeSliderDemo");
	static int F, begin, end{ 100 };
	static bool play;
	if (play)
		F++;
	ImGui::DragInt("F", &F);
	ImGui::DragInt("begin", &begin);
	ImGui::DragInt("end", &end);
	ImGui::Checkbox("play", &play);
	ImGui::Separator();
	Widgets::TimeSlider("timeslider", &F, &play, &begin, &end);
	ImGui::End();
}
