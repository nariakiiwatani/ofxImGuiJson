#pragma once
#include <imgui.h>
#include <vector>
#include <limits>
#include <string>
#include "ofxImGuiJsonCallback.h"

namespace ofxImGuiJson {

template<typename JsonType>
void GetVectorValues(const JsonType *schema, const char *key, int size, std::vector<float> &out, float defVal)
{
	out.assign(size, defVal);
	if(!schema || !schema->contains(key)) return;

	const auto &v = (*schema)[key];
	if(v.is_array()) {
		for(int i = 0; i < size && i < (int)v.size(); i++) {
			if(v[i].is_number()) {
				out[i] = v[i].template get<float>();
			}
		}
	}
	else if(v.is_number()) {
		float val = v.template get<float>();
		for(int i = 0; i < size; i++) {
			out[i] = val;
		}
	}
}

template<typename JsonType>
bool EditNumericArray(JsonType &arr, const JsonType *schema,
					  const CallbackList<JsonType> &callbacks,
					  const std::string &path)
{
	if(!schema || !schema->contains("type") || !schema->contains("size")) return false;
	auto oldValue = arr;
	bool changed = false;

	std::string baseType = (*schema)["type"].template get<std::string>();
	int size = (*schema)["size"].template get<int>();
	bool isFloat = (baseType == "float");
	std::string widget = schema->value("ui:widget", "");

	std::vector<float> defaults(size, 0.0f), minVals(size), maxVals(size), speedVals(size);
	float floatMinDef = -1e9f / 2.0f, floatMaxDef = 1e9f / 2.0f;
	float intMinDef   = (float)(std::numeric_limits<int>::min() / 2);
	float intMaxDef   = (float)(std::numeric_limits<int>::max() / 2);

	GetVectorValues(schema, "default", size, defaults, 0.0f);
	GetVectorValues(schema, "minimum", size, minVals, isFloat ? floatMinDef : intMinDef);
	GetVectorValues(schema, "maximum", size, maxVals, isFloat ? floatMaxDef : intMaxDef);
	GetVectorValues(schema, "speed", size, speedVals, 1);

	if(!arr.is_array()) arr = JsonType::array();
	while((int)arr.size() < size) arr.push_back(isFloat ? defaults[arr.size()] : (int)defaults[arr.size()]);

	std::vector<float> fvals(size);
	std::vector<int>   ivals(size);
	for(int i = 0; i < size; i++) {
		if(isFloat) fvals[i] = arr[i].is_number() ? arr[i].template get<float>() : defaults[i];
		else		ivals[i] = arr[i].is_number_integer() ? arr[i].template get<int>() : (int)defaults[i];
	}

	float totalWidth = ImGui::GetContentRegionAvail().x;
	float spacing = ImGui::GetStyle().ItemSpacing.x;
	float fieldWidth = (totalWidth - spacing * (size - 1)) / size;

	for(int i = 0; i < size; i++) {
		ImGui::PushItemWidth(fieldWidth);
		char label[32]; snprintf(label, sizeof(label), "##%d", i);
		auto oldValue = arr[i];
		bool elemChanged = false;

		if(isFloat) {
			if(widget == "slider") {
				elemChanged = ImGui::SliderFloat(label, &fvals[i], minVals[i], maxVals[i]);
			}
			else if(widget == "drag") {
				elemChanged = ImGui::DragFloat(label, &fvals[i], speedVals[i], minVals[i], maxVals[i]);
			}
			else {
				elemChanged = ImGui::InputFloat(label, &fvals[i]);
			}
		}
		else {
			if(widget == "slider") {
				elemChanged = ImGui::SliderInt(label, &ivals[i], (int)minVals[i], (int)maxVals[i]);
			}
			else if(widget == "drag") {
				elemChanged = ImGui::DragInt(label, &ivals[i], speedVals[i], minVals[i], maxVals[i]);
			}
			else {
				elemChanged = ImGui::InputInt(label, &ivals[i]);
			}
		}

		ImGui::PopItemWidth();
		if(i < size - 1) ImGui::SameLine();

		if(elemChanged) {
			arr[i] = isFloat ? JsonType(fvals[i]) : JsonType(ivals[i]);
			InvokeMatchingCallbacks(callbacks, path + "[" + std::to_string(i) + "]", oldValue, arr[i]);
			changed = true;
		}
	}
	if(changed) {
		InvokeMatchingCallbacks(callbacks, path, oldValue, arr);
	}
	return changed;
}

}
