#pragma once
#include "ofxImGuiJsonDispatcher.h"
#include "ofxImGuiJsonCallback.h"

namespace ofxImGuiJson {

template<typename JsonType>
bool Edit(const char *label, JsonType &data, const JsonType *schema,
		  const CallbackList<JsonType> &callbacks = {})
{
	return EditWithSchema(label, data, schema, callbacks, "");
}

template<typename JsonType>
JsonType GenerateDefault(const JsonType &schema)
{
	JsonType data;
	std::function<void(JsonType&, const JsonType&)> fill = [&](JsonType &d, const JsonType &s) {
		if(!s.is_object() || !s.contains("type")) return;
		std::string type = s["type"].template get<std::string>();

		if(type == "object" && s.contains("properties")) {
			d = JsonType::object();
			for(auto it = s["properties"].begin(); it != s["properties"].end(); ++it) {
				const std::string &k = it.key();
				const auto &v = it.value();

				if(v.contains("default")) {
					d[k] = v["default"];
				}
				else if(v.contains("type")) {
					std::string t = v["type"].template get<std::string>();
					if(t == "object") d[k] = JsonType::object();
					else if(t == "array") d[k] = JsonType::array();
					else if(t == "string") d[k] = "";
					else if(t == "boolean") d[k] = false;
					else d[k] = 0;
				}
				fill(d[k], v);
			}
		}
		else if(type == "array" && s.contains("items")) {
			d = s.contains("default") && s["default"].is_array() ? s["default"] : JsonType::array();
			for(auto& item : d) fill(item, s["items"]);
		}
		else if((type == "int" || type == "float") && s.contains("size")) {
			int size = s["size"].template get<int>();
			d = JsonType::array();
			for(int i = 0; i < size; i++) {
				if(s.contains("default")) {
					if(s["default"].is_array() && i < (int)s["default"].size()) {
						d.push_back(s["default"][i]);
					}
					else if(s["default"].is_number()) {
						d.push_back(s["default"]);
					}
					else {
						d.push_back(0);
					}
				}
				else {
					d.push_back(0);
				}
			}
		}
		else {
			if(s.contains("default")) d = s["default"];
			else if(type == "string") d = "";
			else if(type == "boolean") d = false;
			else if(type == "array") d = JsonType::array();
			else if(type == "object") d = JsonType::object();
			else d = 0;
		}
	};
	fill(data, schema);
	return data;
}

}
