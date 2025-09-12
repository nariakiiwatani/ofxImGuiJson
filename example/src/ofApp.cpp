#include "ofApp.h"

using namespace ofxImGuiJson;

void ofApp::setup() {
	ofSetWindowTitle("ofxImGuiJson Example");
	ofSetFrameRate(60);

	gui.setup();
	schema = ofLoadJson("schema_full.json");
	data = GenerateDefault(schema);
}

void ofApp::draw() {
	gui.begin();
	if (ImGui::Begin("Settings")) {
		Edit("Profile", data, &schema);
	}
	ImGui::End();
	gui.end();
}
