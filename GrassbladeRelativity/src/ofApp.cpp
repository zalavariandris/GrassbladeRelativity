#include "ofApp.h"
#include "examples/grassblade.h"
#include "examples/ThinGrass.h"
//#include "examples/im2d_demos.h"
//#include "examples/json_example.h"
#include "examples/animcurvedemo.h"
#include "examples/pathdemo.h"
#include "examples/polydemo.h"
#include "examples/curvecoordsdemo.h"
#include "examples/vieweritemsdemo.h"

void ofApp::setup() {
	/* imgui */
	ImGuiIO& io = ImGui::GetIO();
	//ImFont* font1 = io.Fonts->AddFontDefault();
	font1 = io.Fonts->AddFontFromFileTTF(ofToDataPath("segoe-ui.ttf").c_str(), 13.0*2);
	auto & style = ImGui::GetStyle();
	style.ScaleAllSizes(2.0);
	style.TabRounding = 0.0;
	style.WindowMenuButtonPosition = ImGuiDir_Right;
	gui.setup();
	
	io.FontGlobalScale = 1.0;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigDockingWithShift = true;
	
	/* OF */
	ofDisableArbTex();
	//ofEnableNormalizedTexCoords(); // needless with disabled ARB textures, but left it here for clarity.
	ofSetVerticalSync(true);
	ofSetBackgroundColor(ofColor(10, 10, 10));
	//ofToggleFullscreen();
}

void ofApp::update() {	
	
}

void ShowDockSpace()
{
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	bool p_open{ true };
	ImGui::SetNextWindowBgAlpha(0.0);
	ImGui::Begin("DockSpace", &p_open, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}
	else
	{
		/*ShowDockingDisabledMessage();*/
	}


	static bool show_style_editor = false;
	static bool show_demo_window = false;
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Docking"))
		{
			// Disabling fullscreen would allow the window to be moved to the front of other windows,
			// which we can't undo at the moment without finer window depth/z control.
			//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

			if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0))                 dockspace_flags ^= ImGuiDockNodeFlags_NoSplit;
			if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))                dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
			if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))  dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
			if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0))     dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
			if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))          dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Windows")) {
			if (ImGui::MenuItem("StyleEditor", "", show_style_editor)) show_style_editor = !show_style_editor;
			if (ImGui::MenuItem("DemoWindow", "", show_demo_window)) show_demo_window = !show_demo_window;
			ImGui::EndMenu();
		}
		ImGui::Text("%.2ffps", ofGetFrameRate());
		ImGui::EndMenuBar();
	}

	ImGui::End();

	if (show_style_editor) ImGui::ShowStyleEditor();
	if (show_demo_window) ImGui::ShowDemoWindow();
}

void ofApp::draw() {
	gui.begin();
	
	ShowDockSpace();

	//static ThinGrass thingrass; thingrass.tick();
	//static AnimcurveDemo animcurvedemo; animcurvedemo.tick();
	//static Grassblade grassbalde; grassbalde.tick();
	//static Pathdemo pathdemo; pathdemo.tick();
	//static Polydemo polydemo; polydemo.tick();
	//static Curvecoordsdemo curvecoordsdemo; curvecoordsdemo.tick();
	static Vieweritemsdemo vieweritemsdemo; vieweritemsdemo.tick();
	//showGrassblade();
	//showJsonExample();

	//static AEKeyframes aeKeyframes;
	//aeKeyframes.show();
	gui.end();
}