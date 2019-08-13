#pragma once
#include "imgui.h"
#include "glm/glm.hpp"

// Draw
glm::vec2 toScreen(glm::vec2 P);

glm::vec2 fromScreen(glm::vec2 P);

void addPoint(glm::vec2 P, ImColor color = ImColor(256, 256, 256), float r = 8);

void addLineSegment(const glm::vec2 & A, const glm::vec2 & B, ImColor color = ImColor(255, 255, 255), float thickness = 1.0);

void addText(glm::vec2 P, char * fmt, ...);

//
void addArrow(const glm::vec2 & A, const glm::vec2 & B, ImColor color, float thickness = 1.0);

void addGrid();

glm::vec2 cubicBezier(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 D, float t);

void addBezierSegment(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 D);
