#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include "Im2D/Im2D.h"
#include "imgui.h"
#include "Im2D/Im2D.h"
#include "Im2D/im2d_draw.h"
#include "Paper/Curve.h"
#include "Paper/Path.h"
#include "Paper/CurveLocation.h"

class ofApp : public ofBaseApp{
public:
	//imgui
	ofxImGui::Gui gui;

	//of
	void setup();
	void draw();
	void update();
};
