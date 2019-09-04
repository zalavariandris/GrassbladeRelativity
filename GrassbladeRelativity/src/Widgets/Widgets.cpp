#include "Widgets.hpp"

#include "imgui.h"
#include "imgui_internal.h"
#include "../Im2D/Im2D.h"
#include "../Im2D/im2d_draw.h"
#include "../Animation/AnimCurve.h"
#include "ofAppRunner.h" //getFrameRate
/* Widgets */

void Widgets::TimeSlider(const char * label_id, int * frame, bool * play, int * begin, int * end) {
	// time slider
	ImGui::BeginGroup();

	int duration = *end - *begin;
	ImGui::PushItemWidth(50);
	ImGui::DragInt("##begin", begin);
	if (*begin >= *end)
		*begin = *end;
	ImGui::SameLine();
	ImVec2 sliderPos = ImGui::GetCursorScreenPos();
	float sliderWidth = ImGui::GetWindowContentRegionWidth() - 2 * (ImGui::GetWindowContentRegionWidth() - ImGui::GetContentRegionAvailWidth());
	ImGui::PushItemWidth(sliderWidth);
	ImGui::SliderInt("##frame", frame, *begin, *end);
	ImGui::SameLine();
	ImGui::PushItemWidth(50);
	ImGui::DragInt("##end", end);
	if (*end <= *begin)
		*end = *begin;

	// draw ticks
	auto window = ImGui::GetCurrentWindow();
	for (int f = *begin; f < *end + 1; f++) {
		float x = (float)f / (duration + 1)*sliderWidth + sliderPos.x;
		window->DrawList->AddLine(ImVec2(x, sliderPos.y), ImVec2(x, sliderPos.y+10), ImColor(255, 255, 255, 60));
	}

	// playback controls
	auto style = ImGui::GetStyle();
	float btnWidth = ImGui::GetFontSize()*1.5;
	float btnHeight = btnWidth;
	ImGui::SetCursorPosX(ImGui::GetContentRegionAvailWidth() / 2 - (btnWidth * 6 + 2 * style.ItemSpacing.x) / 2);
	ImGui::BeginGroup();
	if (ImGui::Button("|<<", ImVec2(btnWidth, btnHeight)))
		(*frame) = (*begin);
	ImGui::SameLine();
	if (ImGui::Button("|<", ImVec2(btnWidth, btnHeight)))
		(*frame)--;
	ImGui::SameLine();
	if (ImGui::Button("||", ImVec2(btnWidth, btnHeight)))
		*play = false;
	ImGui::SameLine();
	if (ImGui::Button(">", ImVec2(btnWidth, btnHeight)))
		*play = true;
	ImGui::SameLine();
	if (ImGui::Button(">|", ImVec2(btnWidth, btnHeight)))
		(*frame)++;
	ImGui::SameLine();
	if (ImGui::Button(">>|", ImVec2(btnWidth, btnHeight)))
		*frame = *end;
	ImGui::SameLine();
	ImGui::Text("%.0ffps", ofGetFrameRate());
	ImGui::EndGroup();//end buttons group
	ImGui::EndGroup();//end timeslider group
}

void Widgets::GraphEditor(const char * label_id, std::vector<Animation::AnimCurve*> curves, int * F) {
	// start viewer
	Im2D::ViewerBegin(label_id, ImVec2(), Im2DViewportFlags_Grid | Im2DViewportFlags_AllowNonUniformZoom);

	/*
		* Handle select and move
		*/
	enum Action {
		ACTION_IDLE,
		ACTION_SELECTING,
		ACTION_MOVING
	};

	// get a single key at specified coordinates
	auto getKeyAtCoords = [&](glm::vec2 coords, double tolerance = 5)->Animation::Key* {
		for (Animation::AnimCurve * curve : curves) {
			for (auto &key : curve->getKeys()) {
				auto P = toScreen({ key.frame, key.value });
				if (ImRect(coords - glm::vec2(tolerance), coords + glm::vec2(tolerance)).Contains(P)) {
					return &key;
				}
			}
		}
		return nullptr;
	};

	// get all keys inside a rect
	auto getKeysInRect = [&](ImRect rect, double tolerance = 5)->std::vector<Animation::Key*> {
		std::vector<Animation::Key*> keysInRect;
		for (Animation::AnimCurve * curve : curves) {
			for (auto &key : curve->getKeys()) {
				auto P = toScreen({ key.frame, key.value });
				if (rect.Contains(P)) {
					keysInRect.push_back(&key);
				}
			}
		}
		return keysInRect;
	};

	static Action action = ACTION_IDLE;
	static glm::vec2 selectionFrom;
	static glm::vec2 selectionTo;
	bool AnyModifierKey = ImGui::GetIO().KeyCtrl || ImGui::GetIO().KeyAlt || ImGui::GetIO().KeyShift;
	if (action == ACTION_IDLE && ImGui::IsItemClicked(0) && !AnyModifierKey) {
		// get keys under mouse
		auto mouseScreenPos = glm::vec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
		Animation::Key * keyHit = getKeyAtCoords(mouseScreenPos);
		bool anyKeyHit = keyHit == nullptr ? false : true;

		//
		if (anyKeyHit) {
			if (keyHit->selected) {
				// start moving selected keys
				action = ACTION_MOVING;
			}
			else {
				//deselect all keys
				for (Animation::AnimCurve * curve : curves)
					for (auto & key : curve->getKeys())
						key.selected = false;

				keyHit->selected = true;
				action = ACTION_MOVING;
			}

		}
		else {
			//deselect all keys
			for (Animation::AnimCurve * curve : curves)
				for (auto & key : curve->getKeys())
					key.selected = false;

			// start rect selection
			action = ACTION_SELECTING;
			selectionFrom.x = ImGui::GetMousePos().x;
			selectionFrom.y = ImGui::GetMousePos().y;
			selectionTo.x = selectionFrom.x;
			selectionTo.y = selectionFrom.y;
		}
	}

	ImRect selectionRect{ 0,0,0,0 };
	if (action == ACTION_SELECTING && ImGui::IsItemActive() && ImGui::IsMouseDragging(0) && !AnyModifierKey) {
		// extend rect selection
		selectionTo.x = ImGui::GetMousePos().x;
		selectionTo.y = ImGui::GetMousePos().y;

		// select keys in rectangle
		selectionRect = ImRect(selectionFrom, selectionTo);
		if (selectionRect.Min.x > selectionRect.Max.x)
			std::swap(selectionRect.Min.x, selectionRect.Max.x);
		if (selectionRect.Min.y > selectionRect.Max.y)
			std::swap(selectionRect.Min.y, selectionRect.Max.y);
		for (Animation::AnimCurve * curve : curves) {
			for (auto &key : curve->getKeys()) {
				auto P = toScreen({ key.frame, key.value });
				if (selectionRect.Contains(P)) {
					key.selected = true;
				}
				else {
					key.selected = false;
				}
			}
		}
	}

	if (action == ACTION_MOVING && ImGui::IsItemActive() && ImGui::IsMouseDragging(0) && !AnyModifierKey) {
		// move keys with drag
		glm::vec2 delta = Im2D::GetMouseDelta();
		for (Animation::AnimCurve * curve : curves) {
			for (auto & key : curve->getKeys()) {
				if (key.selected) {
					key.frame += delta.x;
					key.value += delta.y;
				}
			}
		}
	}
	if (ImGui::IsItemDeactivated()) {
		action = ACTION_IDLE;
	}

	// draw animation curves
	for (Animation::AnimCurve * animCurve : curves) {
		auto & keys = animCurve->getKeys();
		for (auto i = 0; i < keys.size() - 1; i++) {
			Animation::Key keyA = keys.at(i);
			Animation::Key keyB = keys.at(i + 1);

			// draw x curve
			glm::vec2 posAx = glm::vec2(keyA.frame, keyA.value);
			glm::vec2 posBx = glm::vec2(keyB.frame, keyB.value);
			addLineSegment(posAx, posBx);
		}
	}

	// draw animation keys
	for (Animation::AnimCurve * animCurve : curves) {
		auto keys = animCurve->getKeys();
		for (auto key : keys) {
			addRect(glm::vec2(key.frame, key.value), 5, 5, key.selected ? ImColor(255, 255, 255) : ImColor(128, 128, 128), 2.0);
		}
	}

	// draw animCurve labels
	auto window = ImGui::GetCurrentWindow();
	for (Animation::AnimCurve * animCurve : curves) {
		float screenLeft = ImGui::GetWindowPos().x + ImGui::GetStyle().ItemSpacing.x;
		float viewLeft = fromScreen({ screenLeft,0 }).x;
		ImVec2 labelPos;
		auto firstKey = animCurve->getKeys().at(0);
		if (viewLeft > firstKey.frame) {
			float valueAtLeft = animCurve->getValueAtFrame(viewLeft);
			float screenValue = toScreen({ 0, valueAtLeft }).y;
			labelPos = ImVec2(
				screenLeft,
				screenValue
			);
		}
		else {
			auto screenPos = toScreen({ firstKey.frame, firstKey.value });
			labelPos = ImVec2(screenPos.x, screenPos.y);
		}
		labelPos.y -= ImGui::GetFontSize();

		window->DrawList->AddText(labelPos, ImColor(255, 255, 255), animCurve->label.c_str());
	}

	// draw current time
	auto top = fromScreen({ 0, ImGui::GetWindowPos().y }).y;
	auto bottom = fromScreen({ 0, ImGui::GetWindowPos().y + ImGui::GetWindowHeight() }).y;
	addLineSegment({ *F, top }, { *F, bottom }, ImColor(255, 255, 0, 128));

	//draw selection rectangle
	if (action == ACTION_SELECTING) {
		auto window = ImGui::GetCurrentWindow();
		window->DrawList->AddRectFilled(
			selectionRect.Min,
			selectionRect.Max,
			ImColor(255, 255, 255, 25)
		);
		window->DrawList->AddRect(
			selectionRect.Min,
			selectionRect.Max,
			ImColor(255, 255, 255, 200)
		);
	}

	Im2D::ViewerEnd();
}
