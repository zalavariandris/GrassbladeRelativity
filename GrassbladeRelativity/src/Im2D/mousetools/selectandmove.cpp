#include "selectandmove.h"
#include <iostream>
#include "../im2d_draw.h"

namespace {
	Im2DItem * getItemAtPos(glm::vec2 pos, double tolerance=10) {
		auto zoom = Im2D::getZoom();
		auto worldTolerance = glm::vec2(tolerance / zoom.x, tolerance / zoom.y);
		ImRect rect(pos.x - worldTolerance.x / 2, pos.y - worldTolerance.y / 2, pos.x + worldTolerance.x / 2, pos.y + worldTolerance.y / 2);
		auto ctx = Im2D::GetCurrentContext();
		for (auto & item : ctx->items) {
			if (rect.Contains(*item.pos)) {
				return &item;
			}
		}
		return nullptr;
	}

	void deselectAllItems() {
		auto ctx = Im2D::GetCurrentContext();
		for (auto item : ctx->items) {
			*item.selected = false;
		}
	}
}

namespace Im2D {
	void SelectAndMoveItems() {
		// mouse state shortcuts
		bool isMousePressed = ImGui::IsMouseClicked(0) && !ImGui::GetIO().KeyAlt && ImGui::IsWindowHovered();
		bool isMouseDragging = ImGui::IsMouseDragging(0) && !ImGui::GetIO().KeyAlt && ImGui::IsWindowFocused();
		bool isMouseReleased = ImGui::IsMouseReleased(0) && !ImGui::GetIO().KeyAlt && ImGui::IsWindowFocused();

		static Im2DItemsAction action = Im2DItemsAction_SELECT;
		static ImRect selectionRect;
		//
		auto ctx = Im2D::GetCurrentContext();
		if (action == Im2DItemsAction_SELECT) {
			if (isMousePressed) {
				Im2DItem * itemHit = getItemAtPos(Im2D::GetMousePos());

				if (itemHit != nullptr) {
					if (*itemHit->selected) {
						action = Im2DItemsAction_MOVE;
					}
					else {
						deselectAllItems();
						*itemHit->selected = true;
						action = Im2DItemsAction_MOVE;
					}
				}
				else {
					deselectAllItems();
					action = Im2DItemsAction_SELECT;
				}
			}

			if (isMouseDragging) {
				// create sleection reactangle
				auto mouseClickedPos = Im2D::GetMousePos() - Im2D::GetMouseDragDelta();
				selectionRect = ImRect(mouseClickedPos, Im2D::GetMousePos());

				// normalize rectangle
				if (selectionRect.Min.x > selectionRect.Max.x)
					std::swap(selectionRect.Min.x, selectionRect.Max.x);
				if (selectionRect.Min.y > selectionRect.Max.y)
					std::swap(selectionRect.Min.y, selectionRect.Max.y);

				for (auto & item : ctx->items) {
					*item.selected = selectionRect.Contains(*item.pos);
				}
			}

			if (isMouseReleased) {
				selectionRect = ImRect(0, 0, 0, 0);
			}
		}

		if (action == Im2DItemsAction_MOVE) {
			// keep a copy of the original item positions TODO: keep a copy only of the selected items
			static std::vector<glm::vec2> positions(ctx->items.size());

			if (isMousePressed) {
				positions.clear();
				for (auto item : ctx->items) {
					positions.push_back(*item.pos);
				}
			}

			if (isMouseDragging) {
				glm::vec2 offset = Im2D::GetMouseDragDelta();
				std::cout << "move " << offset.x << ", " << offset.y << std::endl;
				for (auto i = 0; i < ctx->items.size(); i++) {
					auto & item = ctx->items[i];
					if (*item.selected) {
						*item.pos = positions[i] + offset;
					}
				}
			}

			if (isMouseReleased) {
				std::cout << "release keys" << std::endl;
				positions.clear();
				action = Im2DItemsAction_SELECT;
			}
		}

		// display selection rectangle
		if (isMouseDragging && action == Im2DItemsAction_SELECT) {
			ImGuiWindow * window = ImGui::GetCurrentWindow();
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

		// TODO: move this the the Viewer. otherwise if Im2D::Item will pollute the ctx forever with items;
		ctx->items.clear();
	}
}