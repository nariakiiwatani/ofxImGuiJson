#pragma once
#include <imgui.h>
#include <string>
#include "ofxImGuiJsonCallback.h"
#include "ofxImGuiJsonDispatcher.h"

namespace ofxImGuiJson {

template<typename JsonType>
bool EditArray(JsonType &arr, const JsonType *schema,
			   const CallbackList<JsonType> &callbacks,
			   const std::string &path)
{
	bool changed = false;
	auto oldValue = arr;
	std::string widget = schema ? schema->value("ui:widget", "") : "";
	const JsonType *itemSchema = (schema && schema->contains("items")) ? &(*schema)["items"] : nullptr;

	int minItems = schema ? schema->value("minItems", 0) : 0;
	int maxItems = schema ? schema->value("maxItems", std::numeric_limits<int>::max()) : std::numeric_limits<int>::max();

	auto drawItem = [&](int i, std::string path) {
		auto oldValue = arr[i];
		ImGui::PushID(i);
		bool changed = false;
		if(EditWithSchema("[]", arr[i], itemSchema, callbacks, path)) {
			InvokeMatchingCallbacks(callbacks, path, oldValue, arr[i]);
			changed |= true;
		}
		ImGui::SameLine();
		if(ImGui::SmallButton("x") && (int)arr.size() > minItems) {
			arr.erase(arr.begin() + i);
			changed |= true;
		}
		ImGui::PopID();
		return changed;
	};
	auto drawAddButton = [&]() {
		if(ImGui::Button("+ add")) {
			arr.push_back(itemSchema && itemSchema->contains("default") ? (*itemSchema)["default"] : "");
			return true;
		}
		return false;
	};

	if(widget == "table") {
		if(ImGui::BeginTable("table", 2, ImGuiTableFlags_Borders)) {
			for(int i = 0; i < (int)arr.size(); ++i) {
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("[%d]", i);
				ImGui::TableSetColumnIndex(1);

				std::string childPath = path + "[" + std::to_string(i) + "]";
				changed |= drawItem(i, childPath);
			}
			if((int)arr.size() < maxItems) {
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				changed |= drawAddButton();
			}
			ImGui::EndTable();
		}
	}
	else if(widget == "chips") {
		auto getRequiredWidth = [](std::string label) {
			float spacing = ImGui::GetStyle().ItemSpacing.x;
			ImVec2 buttonSize = ImGui::CalcTextSize(label.c_str()) + ImGui::CalcTextSize("x");
			buttonSize.x += ImGui::GetStyle().FramePadding.x * 4;
			buttonSize.y += ImGui::GetStyle().FramePadding.y * 2;
			ImVec2 fullSize(buttonSize.x + spacing, buttonSize.y);
			return fullSize.x;
		};
		ImGui::BeginGroup();
		const float max_width = ImGui::GetContentRegionAvail().x + ImGui::GetCursorPosX();
		for(int i = 0; i < (int)arr.size(); ++i) {
			std::string label = arr[i].is_string() ? arr[i].template get<std::string>() : "<item>";
			float width = getRequiredWidth(label);
			if(ImGui::GetCursorPosX() >= max_width - width) {
				ImGui::NewLine();
			}
			ImGui::PushID(i);
			ImGui::Button(label.c_str());
			ImGui::SameLine();
			if(ImGui::SmallButton("x") && (int)arr.size() > minItems) {
				arr.erase(arr.begin() + i);
				changed = true;
			}
			ImGui::SameLine();
			ImGui::PopID();
		}
		ImGui::NewLine();
		if((int)arr.size() < maxItems) {
			changed |= drawAddButton();
		}
		ImGui::EndGroup();
	}
	else {
		for(int i = 0; i < (int)arr.size(); ++i) {
			std::string childPath = path + "[" + std::to_string(i) + "]";
			changed |= drawItem(i, childPath);
		}
		if((int)arr.size() < maxItems) {
			changed |= drawAddButton();
		}
	}
	if(changed) {
		InvokeMatchingCallbacks(callbacks, path, oldValue, arr);
	}
	return changed;
}

}
