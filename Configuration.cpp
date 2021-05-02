#include "Configuration.h"
#include "GUI.h"

namespace gui
{
	Configuration::Configuration(AssetPacker::fileImageMap_t* filesys)
		: w(0),
		h(0),
		filesys(*filesys)
	{
	}

	bool Configuration::loadConfig(std::string filename, GUI& gui)
	{
		std::cout << "Loading config: " << filename << std::endl;

		std::string str(filesys[filename].data, filesys[filename].size);
		nlohmann::json j = nlohmann::json::parse(str);

		findIncludes(j);

		bool success = true;

		success &= readJSON(j, "w", w);
		success &= readJSON(j, "h", h);

		if (success &= checkJSON(j, "colors"))
		{
			colors = j.at("colors");
		}

		if (gui.getFontManager() != nullptr)
		{
			std::string fontPath = "assets/fonts/Open_Sans/OpenSans-Regular.ttf";
			gui.getFontManager()->defaultFont = new Font(&(gui.getFontManager()->ft), fontPath, "OpenSans@24", 24, &gui);

			if (success &= checkJSON(j, "fonts"))
			{
				fonts = j.at("fonts");
				for (auto& i : fonts)
				{

					gui.getFontManager()->addFont(i["name"], i["font"], i["size"], i["weight"], i["outline"]);
				}
			}
		}


		if (success &= checkJSON(j, "classes"))
		{
			classes = j.at("classes");
		}

		if (success)
			config = j;

		return success;
	}

	void Configuration::loadInclude(nlohmann::json& j, std::string filename)
	{
		filename = "assets/" + filename;
		std::string str(filesys[filename].data, filesys[filename].size);
		nlohmann::json includeJson = nlohmann::json::parse(str);
		mergeJsonObjects(j, includeJson);
	}

	void Configuration::findIncludes(nlohmann::json& j)
	{
		for (const auto& i : j.items())
		{
			if (i.key() == "include")
			{
				if (i.value().is_array())
				{
					for (const auto& include : i.value().items())
					{
						loadInclude(j, include.value());
					}
				}
				else
				{
					loadInclude(j, i.value());
				}
			}
			else if (i.value().is_structured())
			{
				findIncludes(i.value());
			}
		}
	}

	bool Configuration::getClass(std::string classname, nlohmann::json& j)
	{
		//std::cout << "Retrieving class: " << classname << std::endl;
		// Search in classes
		for (auto& c : classes)
		{
			std::string tmp;
			// Read for classname
			if (readJSON(c, "class-name", tmp))
			{
				// If we find the correct classname
				if (tmp == classname)
				{
					//std::cout << "Found class: " << classname << std::endl;
					// Store the recursive class properties as a glob
					nlohmann::json currentGlob;
					// If it inherits other classes
					nlohmann::json inheritClasses;
					if (readJSON(c, "inherits", inheritClasses))
					{
						//std::cout << "Class has inherits" << std::endl;
						// For each inherited classes
						for (auto& ic : inheritClasses)
						{
							// Retrieve inherit class as a normal class
							nlohmann::json k;
							if (getClass(ic, k))
							{
								// Glob the inherited class to the current class
								globProperties(currentGlob, k);
							}
						}
					}
					//std::cout << "Printing after inherits" << std::endl;
					//printGlob(currentGlob);

					//std::cout << "Class has properties" << std::endl;
					// Override from dervied
					globProperties(currentGlob, c);
					globProperties(j, currentGlob);

					//printGlob(j);
					// We have found the root class, exit
					return true;
				}
			}
		}
		std::cout << "Unable to find class: " << classname << std::endl;
		return false;
	}

	bool Configuration::getColor(std::string colorName, Color& color)
	{
		if (!colorName.empty())
		{
			if (colorName[0] == '#')
			{
				color = hex2rgb(colorName);
				return true;
			}
			else
			{
				for (auto& i : colors)
				{
					std::string tmp;
					if (readJSON(i, colorName, tmp))
					{
						color = hex2rgb(tmp);
						return true;
					}
				}
			}
		}
		color = { 0,0,0 };
		std::cout << "Unable to find color: " << colorName << std::endl;
		return false;
	}

	std::string Configuration::getColorName(Color& color)
	{
		std::string colorName = colorToString(color.r, color.g, color.b);
		for (const auto& j : colors.items())
			for (const auto& i : j.value().items())
				if (i.value() == colorName)
					return i.key();
		return colorName;
	}

	bool Configuration::isVariable(std::string key)
	{
		if (key.size() > 0)
		{
			if (key[0] == '@' && key[key.size() - 1] == '@')
				return true;
		}
		return 0;
	}

	bool Configuration::isLocalVariable(std::string key)
	{
		if (key.size() > 0)
		{
			if (key[0] == '@' && key[1] == '?' && key[2] == '@')
				return true;
		}
		return 0;
	}

	bool Configuration::isGlobalVariable(std::string key)
	{
		if (key.size() > 0)
		{
			if (key[0] == '@' && key[1] == '@' && key[key.size() - 1] == '@' && key[key.size() - 2] == '@')
				return true;
		}
		return 0;
	}

	nlohmann::json& Configuration::getConfig()
	{
		return config;
	}

}

