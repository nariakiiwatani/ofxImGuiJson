#pragma once
#include <imgui.h>
#include <limits>
#include <string>
#include <cstring>
#include "ofxImGuiJsonCallback.h"

namespace ofxImGuiJson {

template<typename JsonType>
bool EditPrimitive(JsonType &value, const JsonType *schema,
				   const CallbackList<JsonType> &callbacks,
				   const std::string &path)
{
	bool changed = false;
	auto oldValue = value;

	std::string widget = schema ? schema->value("ui:widget", "") : "";
	std::string type;

	if(schema && schema->contains("type")) {
		type = (*schema)["type"].template get<std::string>();
	}
	else {
		if(value.is_boolean()) type = "boolean";
		else if(value.is_number_integer()) type = "int";
		else if(value.is_number_float())   type = "float";
		else if(value.is_string())		 type = "string";
		else							   type = "null";
	}

	if(type == "boolean") {
		if(!value.is_boolean()) value = false;
		bool b = value.template get<bool>();
		if(ImGui::Checkbox("##bool", &b)) { value = b; changed = true; }
	}
	else if(type == "int") {
		if(!value.is_number_integer()) value = 0;
		int v = value.template get<int>();
		int minv = schema ? schema->value("minimum", std::numeric_limits<int>::min() / 2) : std::numeric_limits<int>::min() / 2;
		int maxv = schema ? schema->value("maximum", std::numeric_limits<int>::max() / 2) : std::numeric_limits<int>::max() / 2;
		int speed = schema ? schema->value("speed", 1) : 1;

		if(widget == "slider") {
			if(ImGui::SliderInt("##int", &v, minv, maxv)) { value = v; changed = true; }
		}
		else if(widget == "drag") {
			if(ImGui::DragInt("##int", &v, speed, minv, maxv)) { value = v; changed = true; }
		}
		else {
			if(ImGui::InputInt("##int", &v)) { value = v; changed = true; }
		}
	}
	else if(type == "float") {
		if(!value.is_number_float()) value = 0.0f;
		float v = value.template get<float>();
		float minv = schema ? schema->value("minimum", -1e9f / 2.0f) : -1e9f / 2.0f;
		float maxv = schema ? schema->value("maximum",  1e9f / 2.0f) :  1e9f / 2.0f;
		float speed = schema ? schema->value("speed", 1.0f) : 1.0f;

		if(widget == "slider") {
			if(ImGui::SliderFloat("##float", &v, minv, maxv)) { value = v; changed = true; }
		}
		else if(widget == "drag") {
			if(ImGui::DragFloat("##float", &v, speed, minv, maxv)) { value = v; changed = true; }
		}
		else {
			if(ImGui::InputFloat("##float", &v)) { value = v; changed = true; }
		}
	}
	else if(type == "string") {
		if(!value.is_string()) value = "";
		std::string s = value.template get<std::string>();
		char buf[256]; std::strncpy(buf, s.c_str(), sizeof(buf)); buf[sizeof(buf)-1] = '\0';
		if(ImGui::InputText("##str", buf, sizeof(buf))) { value = std::string(buf); changed = true; }
	}
	else {
		ImGui::TextDisabled("null");
	}

	if(changed) {
		InvokeMatchingCallbacks(callbacks, path, oldValue, value);
	}
	return changed;
}

}
