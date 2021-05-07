#include "ShaderProperties.h"
#include "Utilities.h"

GLint stringToWrap(std::string str)
{
	if (str == "GL_REPEAT")
		return GL_REPEAT;
	return NULL;
}

GLint stringToFilter(std::string str)
{
	if (str == "GL_LINEAR")
		return GL_LINEAR;
	return NULL;
}

gui::ShaderProperties::ShaderProperties()
{
}

gui::ShaderProperties::ShaderProperties(nlohmann::json j)
{
	ConfigManifest config = {
		{
			"shader",
			{"",
			[&](std::string value) { shader = value; },
			[&](std::string fieldName) { return nlohmann::json({{fieldName, shader }}); }}
		},		
		{
			"texture",
			{"",
			[&](std::string value) { texture = value; },
			[&](std::string fieldName) { return nlohmann::json({{fieldName, texture }}); }}
		},
		{
			"GL_TEXTURE_WRAP_S",
			{"GL_REPEAT",
			[&](std::string value) { glTextureWrapS = stringToWrap(value); },
			[&](std::string fieldName) { return nlohmann::json({{fieldName, glTextureWrapS }}); }}
		},		
		{
			"GL_TEXTURE_WRAP_T",
			{"GL_REPEAT",
			[&](std::string value) { glTextureWrapT = stringToWrap(value); },
			[&](std::string fieldName) { return nlohmann::json({{fieldName, glTextureWrapT }}); }}
		},
		{
			"GL_TEXTURE_MIN_FILTER",
			{"GL_LINEAR",
			[&](std::string value) { glTextureMinFilter = stringToFilter(value); },
			[&](std::string fieldName) { return nlohmann::json({{fieldName, glTextureMinFilter }}); }}
		},
		{
			"GL_TEXTURE_MAG_FILTER",
			{"GL_LINEAR",
			[&](std::string value) { glTextureMagFilter = stringToFilter(value); },
			[&](std::string fieldName) { return nlohmann::json({{fieldName, glTextureMagFilter }}); }}
		},
		{
			"iterations",
			{"1",
			[&](std::string value) { iterations = std::atoi(value.c_str()); },
			[&](std::string fieldName) { return nlohmann::json({{fieldName, iterations }}); }}
		},
	};
	config.load(j);
}
