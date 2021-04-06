#pragma once

#include "Color.h"
#include <AssetPacker.h>

namespace gui
{
	class GUI;

	typedef std::map<std::string, std::string> localVariables_t;

	class Configuration
	{
	public:
		Configuration(AssetPacker::fileImageMap_t* filesys);

		bool loadConfig(std::string filename, GUI& gui);

		void loadInclude(nlohmann::json& j, std::string filename);

		void findIncludes(nlohmann::json& j);

		bool getClass(std::string classname, nlohmann::json& j);

		bool getColor(std::string colorName, Color& color);

		std::string getColorName(Color& color);

		static bool isVariable(std::string key);

		static bool isLocalVariable(std::string key);

		static bool isGlobalVariable(std::string key);

		nlohmann::json& getConfig();

		int32_t w, h;
	private:
		AssetPacker::fileImageMap_t filesys;
		nlohmann::json colors;
		nlohmann::json classes;
		nlohmann::json fonts;
		nlohmann::json config;
	};

	

}