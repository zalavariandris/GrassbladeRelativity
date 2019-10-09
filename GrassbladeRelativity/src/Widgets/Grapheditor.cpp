#include "Grapheditor.hpp"
#include "../Im2D/Im2D.h"
#include "../Im2D/im2d_draw.h"
#include "../imgui/imgui_internal.h"
#include <glm/ext.hpp>
void Widgets::Grapheditor(const char * label_id, std::vector<Animation::AnimationCurve*> curves, int * F) {
	Im2D::BeginViewer(label_id, ImVec2(0, 0), Im2DViewportFlags_AllowNonUniformZoom | Im2DViewportFlags_Grid);
	// Mouse Tool
	/*
	* Handle select and move
	*/
	enum Action {
		ACTION_IDLE,
		ACTION_SELECTING,
		ACTION_MOVING
	};

	// get a single key at specified coordinates
	auto getKeyAtCoords = [&](glm::vec2 coords, double tolerance = 5)->Animation::Keyframe* {
		for (Animation::AnimationCurve * curve : curves) {
			for (auto & key : curve->keys()) {
				auto P = toScreen({ key.time(), key.value() });
				if (ImRect(coords - glm::vec2(tolerance), coords + glm::vec2(tolerance)).Contains(P)) {
					return &key;
				}
			}
		}
		return nullptr;
	};

	// get all keys inside a rect
	auto getKeysInRect = [&](ImRect rect, double tolerance = 5)->std::vector<Animation::Keyframe*> {
		std::vector<Animation::Keyframe*> keysInRect;
		for (Animation::AnimationCurve * curve : curves) {
			for (auto &key : curve->keys()) {
				auto P = toScreen({ key.time(), key.value() });
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
		Animation::Keyframe * keyHit = getKeyAtCoords(mouseScreenPos);
		bool anyKeyHit = keyHit == nullptr ? false : true;

		//
		if (anyKeyHit) {
			if (keyHit->selected) {
				// start moving selected keys
				action = ACTION_MOVING;
			}
			else {
				//deselect all keys
				for (Animation::AnimationCurve * curve : curves)
					for (auto & key : curve->keys())
						key.selected = false;

				keyHit->selected = true;
				action = ACTION_MOVING;
			}

		}
		else {
			//deselect all keys
			for (Animation::AnimationCurve * curve : curves)
				for (auto & key : curve->keys())
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
		for (Animation::AnimationCurve * curve : curves) {
			for (auto &key : curve->keys()) {
				auto P = toScreen({ key.time(), key.value() });
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
		for (Animation::AnimationCurve * curve : curves) {
			for (auto & key : curve->keys()) {
				if (key.selected) {
					key.time(key.time() + delta.x);
					key.value(key.value() + delta.y);
				}
			}
		}
	}
	if (ImGui::IsItemDeactivated()) {
		action = ACTION_IDLE;
	}
	// display curves
	auto sample = 1;
	float screenLeft = ImGui::GetWindowPos().x + ImGui::GetStyle().ItemSpacing.x;
	float screenRight = ImGui::GetWindowPos().x + ImGui::GetWindowWidth() + ImGui::GetStyle().ItemSpacing.x;
	for (auto curve : curves) {
		for (auto i = screenLeft; i < screenRight - 1; i += sample) {
			double time0 = fromScreen(glm::vec2(i, 0)).x;
			double time1 = fromScreen(glm::vec2(i + sample, 0)).x;
			double value0 = curve->getValueAtFrame(time0);
			double value1 = curve->getValueAtFrame(time1);
			glm::vec2 A(time0, value0);
			glm::vec2 B(time1, value1);
			addLineSegment(A, B, ImColor(255, 255, 255), 1.0);
		}
	}

	// draw tangents
	auto tangentDisplayLength = 100.0 / glm::length(Im2D::getZoom());
	for (auto curve : curves) {
		for (auto & key : curve->keys()) {
			if (key.selected) {
				glm::vec2 pos(key.time(), key.value());
				auto inTangentVector = glm::normalize(glm::vec2(1.0, key.inTangent()));
				auto outTangentVector = glm::normalize(glm::vec2(1.0, key.inTangent()));

				addLineSegment(pos, pos + inTangentVector * tangentDisplayLength, ImColor(128, 128, 128));
				addLineSegment(pos, pos - outTangentVector * tangentDisplayLength, ImColor(128, 128, 128));
			}
		}
	}

	// draw keys
	for (auto & curve : curves) {
		for (auto & key : curve->keys()) {
			glm::vec2 pos(key.time(), key.value());
			addPoint(pos, key.selected ? ImColor(255, 255, 255) : ImColor(128, 128, 128), 4);
		}
	}

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

	//// edit curves
	//for (auto curve : curves) {
	//	for (auto & key : curve->keys()) {
	//		ImGui::PushID(&key);
	//		glm::vec2 pos(key.time(), key.value());
	//		if (Im2D::DragPoint("##key", &pos)) {
	//			key.time(pos.x);
	//			key.value(pos.y);
	//		}
	//		ImGui::PopID();
	//	}
	//}
	Im2D::EndViewer();
}

