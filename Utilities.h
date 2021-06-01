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
extern bool checkJSON(const nlohmann::json& j, std::string fieldName);

template <typename TYPE>
inline bool readJSON(const nlohmann::json& j, std::string fieldName, TYPE& field, bool debugPrint = false)
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

extern bool readJSONAsString(const nlohmann::json& j, std::string fieldName, std::string& field, bool debugPrint = false);

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

