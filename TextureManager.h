#pragma once

#include <string>
#include <string.h>
#include <vector>
#include "AssetPacker.h"

namespace gui
{
	static const std::string TEXTURES_RELATIVE_PATH = "assets\\tex\\";

	struct Texture
	{
		int width, height, channels;
		unsigned char *data;
		unsigned int id;
		std::string path;
		std::string name;
	};

	struct TextureManager
	{
		TextureManager(AssetPacker::fileImageMap_t *filesys);
		std::vector<Texture *> textures;
		Texture *loadTexturePNG(const char *fname);
		unsigned int loadCubemap(std::vector<std::string> faces);
		Texture *findTexture(std::string path);
		Texture *requireTexture(std::string path);
		void clear();
		AssetPacker::fileImageMap_t filesys;
	};

}
