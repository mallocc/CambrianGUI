#pragma once
#include <GL/glew.h>
#include <nlohmann/json.hpp>

namespace gui
{
	struct ShaderProperties
	{
		ShaderProperties();
		ShaderProperties(nlohmann::json j);
		std::string texture;
		std::string shader;
		GLint glTextureWrapS = GL_REPEAT;
		GLint glTextureWrapT = GL_REPEAT;
		GLint glTextureMinFilter = GL_LINEAR;
		GLint glTextureMagFilter = GL_LINEAR;
		float iterations = 1.0f;
	};
}