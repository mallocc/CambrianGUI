#pragma once

#include <nlohmann/json.hpp>
#include <iostream>


typedef std::function<void(std::string)> initcallback_t;
typedef std::function<nlohmann::json(std::string)> jsoncallback_t;

class ConfigItem
{
public:
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

private:
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

class ConfigList : public std::map<std::string, ConfigItem>
{
public:
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

	void load(const nlohmann::json& j, bool onlyOverrides = false, bool debugPrint = false)
	{
		load(j, *this, onlyOverrides, debugPrint);
	}

	nlohmann::json toJson()
	{
		return toJson(*this);
	}

	static bool readJSONAsString(const nlohmann::json& j, std::string fieldName, std::string& field, bool debugPrint = false)
	{
		if (debugPrint)
		{
			std::cout << "Attempting field name " << fieldName << std::endl;
			std::cout << "JSON " << j.dump(4) << std::endl;
		}
		if (!j.empty() && j.contains(fieldName))
		{
			if (j.at(fieldName).is_string())
				field = j.at(fieldName).get<std::string>();
			else
				field = j.at(fieldName).dump();

			if (debugPrint)
				std::cout << "JSON value found: '" << field << "'" << std::endl;
			return true;
		}
		else
		{
			if (debugPrint)
				std::cout << "JSON doesnt contain field name " << fieldName << std::endl;
		}
		return false;
	}

	static void globProperties(nlohmann::json& j1, const nlohmann::json& j2)
	{
		for (const auto& j : nlohmann::json::iterator_wrapper(j2))
		{
			if (j.key() != "class")
				j1[j.key()] = j.value();
		}
	}

	static void load(const nlohmann::json& j, std::map<std::string, ConfigItem>& fields, bool onlyOverrides = false, bool debugPrint = false)
	{
		for (auto& i : fields)
		{
			ConfigItem& field = i.second;
			if (field.initCallback != nullptr)
			{
				std::string tmp;
				if (!ConfigList::readJSONAsString(j, i.first, tmp, debugPrint))
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
				if (ConfigList::readJSONAsString(k, i.first, tmp))
				{
					if (field.val != tmp)
					{
						ConfigList::globProperties(j, k);
					}
				}
			}
		}
		return j;
	}
};
