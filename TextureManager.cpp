#pragma once
#include <GL/glew.h>
#include "TextureManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <string>
#include <string.h>
#include <vector>
#include "Utilities.h"

using gui::TextureManager;
using gui::Texture;


gui::TextureManager::TextureManager(AssetPacker::fileImageMap_t* filesys)
	:filesys(*filesys)
{
}

Texture *TextureManager::loadTexturePNG(const char *fname)
{
	int x, y, n;
	//unsigned char *data = stbi_load(fname, &x, &y, &n, 0);

	unsigned char* data = stbi_load_from_memory((unsigned char *)filesys[fname].data, filesys[fname].size, &x, &y, &n, 0);

	if (data != nullptr)
	{
		GLuint textureId;
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		std::cout << "Loaded Texture: " << fname << " (" << x << ", " << y << ", " << n << ")" << " texId: " << textureId << std::endl;
		Texture* nt = new Texture();
		nt->channels = n;
		nt->data = data;
		nt->height = y;
		nt->width = x;
		nt->path = fname;
		nt->id = textureId;
		return nt;
	}
	return nullptr;
}
Texture *TextureManager::findTexture(std::string path)
{
	for (auto &i : textures)
	{
		if (i->path == path)
		{
			return i;
			break;
		}
	}
	std::cout <<  "Cannot find texture named: " + std::string(path) << std::endl;
	return nullptr;
}
Texture * TextureManager::requireTexture(std::string path)
{
	if (path == "") return nullptr;
	std::string name = path;
	path = TEXTURES_RELATIVE_PATH + path;
	std::replace(path.begin(), path.end(), '\\', '/');
	Texture *t = findTexture(path);
	if (t == nullptr)
	{
		t = loadTexturePNG(path.c_str());
		if (t != nullptr)
		{
			t->name = name;
			textures.push_back(t);
		}
		return t;
	}
	else
	{
		return t;
	}
	std::cout << "Cannot Load texture named: " + std::string(path) + " (Does not exist on disk)" << std::endl;
	return nullptr;
}

void TextureManager::clear()
{
	std::vector <GLuint> texIds;
	for (auto& i : textures)
	{
		texIds.push_back(i->id);
		delete[] i->data;
		delete i;
	}
	glDeleteTextures(texIds.size(), texIds.data());
	textures.clear();
}

unsigned int TextureManager::loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load_from_memory((unsigned char*)filesys[faces[i]].data, filesys[faces[i]].size, &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
			std::cout << "Loaded cubemap texture: " << faces[i] << std::endl;
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}

	glGenerateTextureMipmap(textureID);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY, 16.0f);

	return textureID;
}
