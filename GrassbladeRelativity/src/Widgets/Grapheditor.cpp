#include "Grapheditor.hpp"
#include "../Im2D/Im2D.h"
#include "../Im2D/im2d_draw.h"
#include "../imgui/imgui_internal.h"
#include <glm/ext.hpp>
#include <iostream>
#include <algorithm> // transform

// Mouse Tool
enum MouseTool {
	SelectAndMoveTool
};

bool SelectAndMove(std::vector<Animation::AnimationCurve*> curves) {
	//std::vector<Im2DItem> items;
	//for (auto curve : curves) {
	//	for (auto key : curve->keys()) {
	//		items.push_back()
	//	}
	//}

	// Helpers
	// get a single key at specified coordinates
	auto getKeyAtCoords = [&](glm::vec2 coords, double tolerance = 10)->Animation::Keyframe* {
		coords = toScreen(coords);
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

	//
	bool changed = false;
	bool isMousePressed = ImGui::IsItemClicked(0) && !ImGui::GetIO().KeyAlt;
	bool isMouseDragging = ImGui::IsItemActive() && ImGui::IsMouseDragging(0) && !ImGui::GetIO().KeyAlt;
	bool isMouseReleased = ImGui::IsItemDeactivated() && !ImGui::GetIO().KeyAlt;

	static ImVec2 mouseDownPoint;
	static ImVec2 mousePoint;
	if (isMousePressed) {
		mousePoint = Im2D::GetMousePos();
		mouseDownPoint = mousePoint;
	}

	if (isMouseDragging) {
		mousePoint = Im2D::GetMousePos();
	}

	if (isMouseReleased) {
		mousePoint = Im2D::GetMousePos();
	}

	/*
	* Handle select and move
	*/
	enum Action {
		ACTION_SELECT,
		ACTION_MOVE
	};

	static Action action = ACTION_SELECT;

	if (action == ACTION_SELECT) {
		ImRect selectionRect{ 0,0,0,0 };
		if (isMousePressed) {
			// get keys under mouse
			Animation::Keyframe * keyHit = getKeyAtCoords(mousePoint);
			bool anyKeyHit = keyHit == nullptr ? false : true;

			//
			if (anyKeyHit) {
				if (keyHit->selected) {
					// start moving selected keys
					action = ACTION_MOVE;
				}
				else {
					//deselect all keys
					for (Animation::AnimationCurve * curve : curves)
						for (auto & key : curve->keys())
							key.selected = false;

					keyHit->selected = true;
					action = ACTION_MOVE;
				}
			}
			else {
				//deselect all keys
				for (Animation::AnimationCurve * curve : curves)
					for (auto & key : curve->keys())
						key.selected = false;

				// start rect selection
				action = ACTION_SELECT;
			}
		}

		if (isMouseDragging) {
			// create sleection reactangle
			selectionRect = ImRect(mouseDownPoint, mousePoint);

			// normalize rectangle
			if (selectionRect.Min.x > selectionRect.Max.x)
				std::swap(selectionRect.Min.x, selectionRect.Max.x);
			if (selectionRect.Min.y > selectionRect.Max.y)
				std::swap(selectionRect.Min.y, selectionRect.Max.y);

			// select keys inside rectangle
			for (Animation::AnimationCurve * curve : curves) {
				for (auto &key : curve->keys()) {
					auto P = ImVec2(key.time(), key.value());
					if (selectionRect.Contains(P)) {
						key.selected = true;
					}
					else {
						key.selected = false;
					}
				}
			}

			// draw selection rect
			auto window = ImGui::GetCurrentWindow();
			window->DrawList->AddRectFilled(
				toScreen(selectionRect.Min),
				toScreen(selectionRect.Max),
				ImColor(255, 255, 255, 25)
			);
			window->DrawList->AddRect(
				toScreen(selectionRect.Min),
				toScreen(selectionRect.Max),
				ImColor(255, 255, 255, 200)
			);
		}
		if (isMouseReleased) {
			std::cout << "release selecting" << std::endl;
		}
	}

	if (action == ACTION_MOVE) {
		// keep a copy of the original curves TODO: keep a copy only of the selected keys
		static std::vector<Animation::AnimationCurve> bufferCurves;

		//
		if (isMousePressed) {
			std::cout << "start moving keys" << std::endl;
			// keep a copy of the curves when start moving keys
			std::transform(curves.begin(), curves.end(), std::back_inserter(bufferCurves), [](Animation::AnimationCurve * curve) {
				return *curve;
			});
		}

		if (isMouseDragging) {
			std::cout << "moving keys" << std::endl;
			// move keys with drag
			
			auto offset = ImVec2(mousePoint.x- mouseDownPoint.x, mousePoint.y-mouseDownPoint.y);
			for (auto c = 0; c < curves.size(); c++) {
				for (auto k = 0; k < curves[c]->keys().size(); k++) {
					auto & key = curves[c]->keys()[k];
					auto & bufferKey = bufferCurves[c].keys()[k];
					if (key.selected) {
						key.time(bufferKey.time() + offset.x);
						key.value(bufferKey.value() + offset.y);
					}
				}
			}

			changed = true;
		}

		if (isMouseReleased) {
			std::cout << "release keys" << std::endl;
			bufferCurves.clear();
			action = ACTION_SELECT;
		}
	}

	return changed;
}

void Widgets::Grapheditor(const char * label_id, std::vector<Animation::AnimationCurve*> curves, int * F) {
	Im2D::BeginViewer(label_id, ImVec2(0, 0), Im2DViewportFlags_AllowNonUniformZoom | Im2DViewportFlags_Grid);

	static auto mouseTool = SelectAndMoveTool;

	if (mouseTool == SelectAndMoveTool) {
		SelectAndMove(curves);
	}

	// display curves
	double sample = 1;
	float screenLeft = ImGui::GetWindowPos().x + ImGui::GetStyle().ItemSpacing.x;
	float screenRight = ImGui::GetWindowPos().x + ImGui::GetWindowWidth() + ImGui::GetStyle().ItemSpacing.x;
	auto tangentDisplayLength = 100.0 / glm::length(Im2D::getZoom());

	for (auto curve : curves) {
		for (auto i = screenLeft; i < screenRight; i += sample) {
			double time0 = fromScreen(glm::vec2(i, 0)).x;
			double time1 = fromScreen(glm::vec2(i + sample, 0)).x;
			double value0 = curve->getValueAtFrame(time0);
			double value1 = curve->getValueAtFrame(time1);
			glm::vec2 A(time0, value0);
			glm::vec2 B(time1, value1);
			addLineSegment(A, B, ImColor(255, 255, 255), 1.0);
		}

		// draw keyframes
		for (auto & key : curve->keys()) {
			// draw keys
			glm::vec2 pos(key.time(), key.value());
			addPoint(pos, key.selected ? ImColor(255, 255, 255) : ImColor(128, 128, 128), 4);

		//	//draw tangents
		//	if (key.selected) {
		//		glm::vec2 pos(key.time(), key.value());
		//		auto inTangentVector = glm::normalize(glm::vec2(1.0, key.inTangent()));
		//		auto outTangentVector = glm::normalize(glm::vec2(1.0, key.inTangent()));

		//		addLineSegment(pos, pos + inTangentVector * tangentDisplayLength, ImColor(128, 128, 128));
		//		addLineSegment(pos, pos - outTangentVector * tangentDisplayLength, ImColor(128, 128, 128));
		//	}
		}
	}

	Im2D::EndViewer();
}

