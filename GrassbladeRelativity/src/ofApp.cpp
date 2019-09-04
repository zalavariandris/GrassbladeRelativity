#include "ofApp.h"
#include "Paper/numerical.h"
#include "Im2DPaper/Im2DPaper.h"
#include "imgui_internal.h"
#include "Reader.h"
#include "utilities.h"
#include "examples/grassblade.h"

// basic frameworks stuff
void ofApp::setup() {
	// imgui
	gui.setup();
	ImGui::GetIO().FontGlobalScale = 1.0;
	
	//OF texture handling
	ofDisableArbTex();
	//ofEnableNormalizedTexCoords(); // needless with disabled ARB textures, but left it here for clarity.
	ofSetVerticalSync(true);
	ofSetBackgroundColor(ofColor(10, 10, 10));

	//ofToggleFullscreen();
}

void ofApp::update() {
	
}

void ofApp::draw() {
	gui.begin();
	showGrassblade();
	gui.end();
}