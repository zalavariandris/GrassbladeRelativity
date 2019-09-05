#include "ofApp.h"
//#include "examples/grassblade.h"
//#include "examples/im2d_demos.h"
#include "examples/json_example.h"

void ofApp::setup() {
	/* imgui */
	gui.setup();
	ImGui::GetIO().FontGlobalScale = 1.0;
	
	/* OF */
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
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("StyleEditor"))
		{
			ImGui::ShowStyleEditor();
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	//showGrassblade();
	showJsonExample();

	gui.end();
}