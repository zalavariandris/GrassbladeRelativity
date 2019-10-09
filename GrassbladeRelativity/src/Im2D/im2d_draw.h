#pragma once
#include "imgui.h"
#include "glm/glm.hpp"

// Draw

glm::vec2 toScreen(glm::vec2 P);
glm::vec2 fromScreen(glm::vec2 P);

void addPoint(glm::vec2 P, ImColor color = ImColor(255, 255, 255), float r = 8);

void addLineSegment(const glm::vec2 & A, const glm::vec2 & B, ImColor color = ImColor(255, 255, 255), float thickness = 1.0);

void addText(glm::vec2 P, const char * fmt, ...);

//
void addArrow(const glm::vec2 & A, const glm::vec2 & B, ImColor color, float thickness = 1.0);

void addAdaptiveGrid();

glm::vec2 cubicBezier(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 D, double t);

//void addBezierSegment(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 D);

void addBezierCurve(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 D, 
	ImColor color = ImColor(255, 255, 255), float thickness = 1.0, int num_segments = 0);

void addRect(glm::vec2 center, float width, float height, ImColor color=ImColor(255,255,255), float thickness=1.0);
