#pragma once
#include <imgui.h>
#include <string>
#include "ofxImGuiJsonCallback.h"
#include "ofxImGuiJsonDispatcher.h"

namespace ofxImGuiJson {

template<typename JsonType>
bool EditObject(JsonType &data, const JsonType *schema,
				const CallbackList<JsonType> &callbacks,
				const std::string &path)
{
	bool changed = false;
	const JsonType *props = (schema && schema->contains("properties")) ? &(*schema)["properties"] : nullptr;
	std::string widget = schema ? schema->value("ui:widget", "") : "";

	auto drawProps = [&]() {
		for(auto it = data.begin(); it != data.end(); ++it) {
			const std::string &key = it.key();
			auto &val = it.value();
			const JsonType *sch = (props && props->contains(key)) ? &(*props)[key] : nullptr;
			std::string childPath = path.empty() ? key : path + "/" + key;
			std::string label  = sch ? sch->value("label", key) : key;

			ImGui::PushID(key.c_str());
			ImGui::Text("%s", label.c_str());
			ImGui::SameLine();
			changed |= EditWithSchema(key.c_str(), val, sch, callbacks, childPath);
			ImGui::PopID();
		}
	};

	if(widget == "collapsible") {
		if(ImGui::TreeNode(schema && schema->contains("label") ? (*schema)["label"].template get<std::string>().c_str() : "Object")) {
			drawProps();
			ImGui::TreePop();
		}
	}
	else if(widget == "inline") {
		ImGui::BeginGroup();
		drawProps();
		ImGui::EndGroup();
	}
	else {
		drawProps();
	}
	return changed;
}

}
