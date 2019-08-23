#pragma once
#include "im2d_draw.h"
#include "Im2D.h"
#include "imgui_internal.h"

// Draw
glm::vec2 toScreen(glm::vec2 P) {
	Im2DContext * ctx = Im2D::GetCurrentContext();
	glm::mat3 projectionView = ctx->projectionMatrix * ctx->viewMatrix;

	glm::vec3 pos = projectionView * glm::vec3(P.x, P.y, 1);
	return glm::vec2(pos.x, pos.y);
}

glm::vec2 fromScreen(glm::vec2 P) {
	Im2DContext * ctx = Im2D::GetCurrentContext();
	glm::mat3 projectionView = ctx->projectionMatrix * ctx->viewMatrix;
	return glm::vec3(P, 1) * glm::transpose(glm::inverse(projectionView));
}

void addPoint(glm::vec2 P, ImColor color, float r) {
	ImGuiWindow * window = ImGui::GetCurrentWindow();
	glm::vec2 P1 = toScreen(P);
	window->DrawList->AddCircleFilled(ImVec2(P1.x, P1.y), r, color);
}

void addLineSegment(const glm::vec2 & A, const glm::vec2 & B, ImColor color, float thickness) {
	ImGuiWindow * window = ImGui::GetCurrentWindow();
	glm::vec2 A1 = toScreen(A);
	glm::vec2 B1 = toScreen(B);
	window->DrawList->AddLine(ImVec2(A1.x, A1.y), ImVec2(B1.x, B1.y), color, thickness);
}

void addText(glm::vec2 P, char * fmt, ...) {
	ImGuiWindow * window = ImGui::GetCurrentWindow();
	char buffer[256];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, 255, fmt, args);
	va_end(args);
	glm::vec2 P1 = toScreen(P);
	char * buffer_end = strstr(buffer, "##");
	if (buffer_end) {
		window->DrawList->AddText(ImVec2(P1.x, P1.y), ImColor(255, 255, 255), buffer, buffer_end);
	}
	else {
		window->DrawList->AddText(ImVec2(P1.x, P1.y), ImColor(255, 255, 255), buffer);
	}
}

//
void addArrow(const glm::vec2 & A, const glm::vec2 & B, ImColor color, float thickness) {
	float scale = Im2D::GetCurrentContext()->viewMatrix[0][0];
	addLineSegment(A, B, color, thickness);
	glm::vec2 V(glm::normalize(B - A) / scale);
	glm::vec2 U(-V.y, V.x);
	float head = 10;
	addLineSegment(B, B + (+U - V)*head, color, thickness);
	addLineSegment(B, B + (-U - V)*head, color, thickness);
}

void addGrid() {
	// Draw adaptive grid
	float scale = Im2D::GetCurrentContext()->viewMatrix[0][0];
	int step = pow(10, ceil(log10(1 / scale))) * 10;

	//float t = ceil(log10(1 / scale)) - log10(1 / scale);

	//ImGui::Text("%.2f, %.2f", t, 1 - t);
	glm::vec2 boundary_min;
	glm::vec2 boundary_max;
	glm::vec2 window_pos{ ImGui::GetWindowPos().x, ImGui::GetWindowPos().y };
	glm::vec2 window_size{ ImGui::GetWindowSize().x, ImGui::GetWindowSize().y };

	boundary_min = fromScreen(window_pos);
	boundary_max = fromScreen(window_pos + window_size);
	boundary_min = glm::vec2(floor(boundary_min.x / step)*step, floor(boundary_min.y / step)*step);
	boundary_max = glm::vec2(ceil(boundary_max.x / step)*step, ceil(boundary_max.y / step)*step);

	for (int x = boundary_min.x; x < boundary_max.x; x += step) {
		glm::vec2 A = glm::vec2(x, boundary_min.y);
		glm::vec2 B = glm::vec2(x, boundary_max.y);
		addLineSegment(A, B, ImColor(128, 128, 128, 56), x % (step * 10) ? 1 : 2);
	}

	for (int y = boundary_min.y; y < boundary_max.y; y += step) {
		glm::vec2 A = glm::vec2(boundary_min.x, y);
		glm::vec2 B = glm::vec2(boundary_max.x, y);
		addLineSegment(A, B, ImColor(128, 128, 128, 56), y % (step * 10) ? 1 : 2);
	}

	addArrow(glm::vec2(), glm::vec2(step * 5, 0), ImColor(255, 0, 0, 100));
	addArrow(glm::vec2(), glm::vec2(0, step * 5), ImColor(0, 255, 0, 100));
}

glm::vec2 cubicBezier(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 D, float t) {
	glm::vec2 P1 = glm::mix(A, B, t);
	glm::vec2 Q1 = glm::mix(B, C, t);
	glm::vec2 R1 = glm::mix(C, D, t);

	glm::vec2 P2 = glm::mix(P1, Q1, t);
	glm::vec2 Q2 = glm::mix(Q1, R1, t);

	glm::vec2 P3 = glm::mix(P2, Q2, t);
	return P3;
}

void addBezierSegment(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 D) {
	const int segments{ 16 };
	glm::vec2 points[segments + 1];
	for (auto i = 0; i < segments + 1; i++) {
		glm::vec2 P = cubicBezier(A, B, C, D, (float)i / segments);
		points[i] = P;
	}
	for (auto i = 0; i < segments; i++) {
		addLineSegment(points[i], points[i + 1], ImColor(255, 255, 255, 255), 1.0);
	}
}