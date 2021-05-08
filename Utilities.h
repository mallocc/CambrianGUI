#pragma once
#include <iostream>
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <nlohmann/json.hpp>

#include <sstream>

extern void createConsole();
extern void outputToDebugFile(std::string in);
extern HMODULE GetCurrentModule();
extern std::string ExePath();
extern std::string DLLPath();
extern bool checkJSON(nlohmann::json j, std::string fieldName);

template <typename TYPE>
inline bool readJSON(nlohmann::json j, std::string fieldName, TYPE& field, bool debugPrint = false)
{
	if (checkJSON(j, fieldName))
	{
		field = j.at(fieldName).get<TYPE>();
		if (debugPrint)
			std::cout << "DEGUB read: " << fieldName << " = " << field << std::endl;
		return true;
	}
	else
	{
		if (debugPrint)
			std::cout << "Failed to read: " << fieldName << std::endl;
	}
	return false;
}

extern bool readJSONAsString(nlohmann::json j, std::string fieldName, std::string& field, bool debugPrint = false);

extern void printGlob(nlohmann::json& j1);

extern void globProperties(nlohmann::json& j1, const nlohmann::json& j2);

extern void mergeJsonObjects(nlohmann::json& j1, const nlohmann::json& j2);

extern std::string colorToString(float r, float g, float b);

extern std::vector<std::string> splitWithCharacters(const std::string& str, int splitLength);

extern void replaceVariables(nlohmann::json& j, std::string key, std::string val);

extern void gatherInstances(std::string fieldName, const nlohmann::json& j, std::vector<nlohmann::json>& instances);

/**
	Create a new scope when getting a string from a json field.
	json_get_string(JSON_SYMBOL, FIELD_NAME_SYMBOL, DATA_CONTAINER_SYMBOL)
	>	std::string DATA_SYMBOL;
	>	if (readJSONAsString(JSON_SYMBOL, FIELD_NAME_SYMBOL, DATA_CONTAINER_SYMBOL))
*/
#define json_get_string(JSON_SYMBOL, FIELD_NAME_SYMBOL, DATA_CONTAINER_SYMBOL) \
	std::string DATA_CONTAINER_SYMBOL; \
	if (readJSONAsString(JSON_SYMBOL, FIELD_NAME_SYMBOL, DATA_CONTAINER_SYMBOL))

/**
	Create a new scope when getting a json object from a json field.
	json_get_object(JSON_SYMBOL, FIELD_NAME_SYMBOL, DATA_CONTAINER_SYMBOL)
	>	nlohmann::json DATA_CONTAINER_SYMBOL;
	>	if (readJSON(JSON_SYMBOL, FIELD_NAME_SYMBOL, DATA_CONTAINER_SYMBOL))
*/
#define json_get_object(JSON_SYMBOL, FIELD_NAME_SYMBOL, DATA_CONTAINER_SYMBOL) \
	nlohmann::json DATA_CONTAINER_SYMBOL; \
	if (readJSON(JSON_SYMBOL, FIELD_NAME_SYMBOL, DATA_CONTAINER_SYMBOL))




typedef std::function<void(std::string)> initcallback_t;
typedef std::function<nlohmann::json(std::string)> jsoncallback_t;

struct ConfigItem
{
	std::string val;
	initcallback_t initCallback = nullptr;
	jsoncallback_t jsonCallback = nullptr;

	ConfigItem() {}

	ConfigItem(
		std::string val,
		initcallback_t initCallback,
		jsoncallback_t jsonCallback)
		: val(val),
		initCallback(initCallback),
		jsonCallback(jsonCallback)
	{
	}

	template<typename T>
	ConfigItem(T& reference, std::string defaultValue)
	{
		init(reference, defaultValue);
	}

	template<typename T>
	ConfigItem(T& reference)
	{
		init(reference);
	}

	template <typename T>
	constexpr auto checkValidReferenceType() -> void
	{
		static_assert(
			std::is_same_v<T, uint8_t> || std::is_same_v<T, uint16_t> || std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t> ||
			std::is_same_v<T, int8_t> || std::is_same_v<T, int16_t> || std::is_same_v<T, int32_t> || std::is_same_v<T, int64_t> ||
			std::is_same_v<T, float> || std::is_same_v<T, double> ||
			std::is_same_v<T, bool> ||
			std::is_same_v<T, std::string> ||
			std::is_same_v<T, nlohmann::json>,
			"Invalid type for config reference variable. See compiler notes/messages to locate the conifg assignment line.");
	}

	template <typename T>
	void init(T& reference, std::string defaultValue = "")
	{
		checkValidReferenceType<T>();

		if (initCallback == nullptr)
		{
			if constexpr (std::is_same_v<T, nlohmann::json>)
			{
				if (defaultValue.empty())
					defaultValue = "{}";
				initCallback = [&](std::string value) {
					reference = nlohmann::json::parse(value);
				};
			}
			else if constexpr (std::is_same_v<T, std::string>)
			{
				if (defaultValue.empty())
					defaultValue = "";
				initCallback = [&](std::string value) {
					reference = value;
				};
			}
			else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
			{
				if (defaultValue.empty())
					defaultValue = "0";
				initCallback = [&](std::string value) {
					reference = std::atof(value.c_str());
				};
			}
			else if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, int16_t> || std::is_same_v<T, int32_t> || std::is_same_v<T, int64_t>)
			{
				if (defaultValue.empty())
					defaultValue = "0";
				initCallback = [&](std::string value) {
					reference = std::atoi(value.c_str());
				};
			}
			else if constexpr (std::is_same_v<T, uint8_t> || std::is_same_v<T, uint16_t> || std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t>)
			{
				if (defaultValue.empty())
					defaultValue = "0";
				initCallback = [&](std::string value) {
					reference = std::stoul(value.c_str());
				};
			}
			else if constexpr (std::is_same_v<T, bool>)
			{
				if (defaultValue.empty())
					defaultValue = "false";
				initCallback = [&](std::string value) {
					reference = value == "true";
				};
			}
		}

		if (jsonCallback == nullptr)
		{
			if constexpr (std::is_same_v<T, nlohmann::json>)
			{
				jsonCallback = [&](std::string value) {
					return nlohmann::json({ { value, (reference).dump(4) } });
				};
			}
			else if constexpr (
				std::is_same_v<T, uint8_t> || std::is_same_v<T, uint16_t> || std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t> ||
				std::is_same_v<T, int8_t> || std::is_same_v<T, int16_t> || std::is_same_v<T, int32_t> || std::is_same_v<T, int64_t> ||
				std::is_same_v<T, float> || std::is_same_v<T, double> ||
				std::is_same_v<T, bool> ||
				std::is_same_v<T, std::string>)
			{
				jsonCallback = [&](std::string value) {
					return nlohmann::json({ {value, reference } });
				};
			}
		}

		this->val = defaultValue;
	}
};
struct ConfigList : std::map<std::string, ConfigItem>
{
	using std::map<std::string, ConfigItem>::map;

	ConfigItem& operator()(std::string&& fieldName)
	{
		return (*this)[fieldName];
	}

	ConfigList& operator+(ConfigList additions)
	{
		insert(additions.begin(), additions.end());
		return *this;
	}

	ConfigList& operator+=(ConfigList additions)
	{
		return (*this) + additions;
	}

	void load(nlohmann::json& j, bool onlyOverrides = false, bool debugPrint = false)
	{
		load(j, *this, onlyOverrides, debugPrint);
	}

	nlohmann::json toJson()
	{
		return toJson(*this);
	}

	static void load(nlohmann::json& j, std::map<std::string, ConfigItem>& fields, bool onlyOverrides = false, bool debugPrint = false)
	{
		for (auto& i : fields)
		{
			ConfigItem& field = i.second;
			if (field.initCallback != nullptr)
			{
				std::string tmp;
				if (!readJSONAsString(j, i.first, tmp, debugPrint))
				{
					tmp = field.val;
					if (onlyOverrides)
						continue;
				}

				field.initCallback(tmp);

				if (debugPrint)
					std::cout << "Loaded value: '" << i.first << " = " << tmp << "'" << std::endl;
			}
		}
	}

	static nlohmann::json toJson(std::map<std::string, ConfigItem> fields)
	{
		nlohmann::json j;
		for (auto& i : fields)
		{
			ConfigItem& field = i.second;
			if (field.jsonCallback != nullptr)
			{
				nlohmann::json k = field.jsonCallback(i.first);
				std::string tmp;
				if (readJSONAsString(k, i.first, tmp))
				{
					if (field.val != tmp)
					{
						globProperties(j, k);
					}
				}
			}
		}
		return j;
	}
};
