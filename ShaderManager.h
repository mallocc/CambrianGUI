#pragma once
#include <GL/glew.h>
#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <map>
#include "AssetPacker.h"

struct ShaderManager
{
	ShaderManager(AssetPacker::fileImageMap_t *filesys);
	GLuint getShader(std::string name);
	void flushShaders();

	AssetPacker::fileImageMap_t filesys;
	std::map<std::string, GLuint> shaders;
};
