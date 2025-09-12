#pragma once
#include <string>
#include <regex>
#include <variant>
#include <functional>
#include <vector>

namespace ofxImGuiJson {

using PathPattern = std::variant<std::string, std::regex>;

template<typename JsonType, typename Fn>
auto WrapCallback(Fn fn) {
	return [fn](const std::string &path, const JsonType &oldVal, const JsonType &newVal) {
		if constexpr (std::is_invocable_v<Fn, const std::string&, const JsonType&, const JsonType&>) fn(path, oldVal, newVal);
		else if constexpr (std::is_invocable_v<Fn, const std::string&, const JsonType&>) fn(path, newVal);
		else if constexpr (std::is_invocable_v<Fn, const std::string&>) fn(path);
		else if constexpr (std::is_invocable_v<Fn, const JsonType&, const JsonType&>) fn(oldVal, newVal);
		else if constexpr (std::is_invocable_v<Fn, const JsonType&>) fn(newVal);
		else if constexpr (std::is_invocable_v<Fn>) fn();
	};
}

template<typename JsonType>
struct PathCallback {
	std::vector<PathPattern> paths;
	std::function<void(const std::string&, const JsonType&, const JsonType&)> callback;

	template<typename Fn>
	PathCallback(std::initializer_list<PathPattern> p, Fn fn)
		: paths(p), callback(WrapCallback<JsonType>(std::move(fn))) {}
};

template<typename JsonType>
using CallbackList = std::vector<PathCallback<JsonType>>;

template<typename JsonType>
static void InvokeMatchingCallbacks(const CallbackList<JsonType> &list,
									const std::string &path,
									const JsonType &oldVal,
									const JsonType &newVal) {
	for(auto &&entry : list) {
		bool matched = entry.paths.empty();
		for(auto &&pat : entry.paths) {
			if(std::holds_alternative<std::regex>(pat)) {
				if(std::regex_match(path, std::get<std::regex>(pat))) {
					matched = true; break;
				}
			}
			else {
				std::string pattern = std::get<std::string>(pat);
				std::string regexStr = "^" + std::regex_replace(pattern, std::regex(R"(\*)"), ".*") + "$";
				if(std::regex_match(path, std::regex(regexStr))) {
					matched = true; break;
				}
			}
		}
		if(matched && entry.callback) {
			entry.callback(path, oldVal, newVal);
		}
	}
}

}
