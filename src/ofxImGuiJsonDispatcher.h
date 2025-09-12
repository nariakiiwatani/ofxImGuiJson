#pragma once
#include "ofxImGuiJsonPrimitiveEditor.h"
#include "ofxImGuiJsonNumericArrayEditor.h"
#include "ofxImGuiJsonObjectEditor.h"
#include "ofxImGuiJsonArrayEditor.h"

namespace ofxImGuiJson {

template<typename JsonType>
bool EditWithSchema(const char *label, JsonType &data, const JsonType *schema,
					const CallbackList<JsonType> &callbacks,
					const std::string &path)
{
	bool changed = false;
	ImGui::PushID(label);

	std::string type = schema && schema->contains("type") ? (*schema)["type"].template get<std::string>() : "";
	int size = schema && schema->contains("size") ? (*schema)["size"].template get<int>() : 0;

	if((type == "float" || type == "int") && size > 0) {
		changed |= EditNumericArray(data, schema, callbacks, path);
	}
	else if(data.is_object() || type == "object") {
		changed |= EditObject(data, schema, callbacks, path);
	}
	else if(data.is_array() || type == "array") {
		changed |= EditArray(data, schema, callbacks, path);
	}
	else {
		changed |= EditPrimitive(data, schema, callbacks, path);
	}

	ImGui::PopID();
	return changed;
}

}
