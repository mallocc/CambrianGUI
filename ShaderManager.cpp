#include "ShaderManager.h"
#include "GLShader.h"
#include <filesystem>
#include "Utilities.h"

ShaderManager::ShaderManager(AssetPacker::fileImageMap_t *filesys)
	:filesys(*filesys)
{
}

GLuint ShaderManager::getShader(std::string name)
{
	if (shaders.find(name) == shaders.end())
	{
		std::string preName = name;
		//name = "";
#define PRODUCTION_RELEASE
#ifdef PRODUCTION_RELEASE
		std::string vertex_name = std::string(name + "_v.glsl");
		std::string fragment_name = std::string(name + "_f.glsl");
		std::cout << "Loading Shader: " << vertex_name << " (" << filesys[vertex_name].size << " bytes) & " << fragment_name << " (" << filesys[fragment_name].size << " bytes)" << std::endl;
		shaders[name] = LoadShaderFromData(filesys[vertex_name].data, filesys[vertex_name].size, filesys[fragment_name].data, filesys[fragment_name].size);
#else
		std::filesystem::path pa = __FILE__;
		pa = pa.parent_path() /= name;

		name = pa.string();

		std::string geom = std::string(name + "_g.glsl").c_str();
		std::string geomParam = "";
		if (std::filesystem::exists(geom))
			geomParam = geom;

		shaders[preName] = LoadShader(std::string(name + "_v.glsl").c_str(), std::string(name + "_f.glsl").c_str(), geomParam.c_str());
		return shaders[preName];

#endif // PRODUCTION_RELEASE

		return shaders[name];
	}

	return shaders[name];
}

void ShaderManager::flushShaders()
{
	shaders.clear();
}
