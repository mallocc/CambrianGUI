#pragma once
#include <iostream>
#include <ft2build.h>
#include FT_GLYPH_H

#include <string>
#include <vector>
#include <map>

#include "AssetPacker.h"
#include <GL/glew.h>

namespace gui
{
	struct FontManager;
	class GUI;

	struct coord_t { float x; float y; };
	typedef std::pair<coord_t, coord_t> line_t;

	struct FontAtlasEntry
	{
		float x[2];
		float y[2];
	};

	struct Character
	{
		float Size[2];
		float Bearing[2];
		GLuint Advance;
		FontAtlasEntry ae;
		char* glyphData;
	};


	struct FontAtlas
	{
		FontAtlas();

		FontAtlasEntry addGlyph(char* buf, int w, int h);
		void precalcGlyph(int w, int h);
		void finishPrecalc();
		void writeAtlas(std::string fname);

		void realloc();
		void free();

		int width;
		int height;
		int margin;
		char* data;

		int cursorX = 0;
		int cursorY = 0;
		int maxY = 0;
		int tallestGlyphHeight = 0;
		GLuint textureID;
	};

	struct LineInfo
	{
		float width;
		std::string text;
	};


	struct Font
	{
		Font(FT_Library *ft, std::string fontPath, std::string fontName, int size, GUI* guiPtr, float weight = 0.f, bool outline = 0);

		FontAtlas* fontAtlas;

		std::map<uint32_t, Character> characters;
		std::string fontPath;
		std::string fontName;
		int size;
		void layoutText(std::string text, float scale, float textSpacing, gui::coord_t bdims, std::vector<LineInfo>* layout);
		void renderText(std::string text, float x, float y, float scale, bool bold = 0, bool italic = 0, int justify = 0, float lineHeight = 1, float textSpacing = 1., coord_t bdims = { 0,0 });
		void renderTextUnicode(std::wstring text, float x, float y, float scale);
		coord_t textMetrics(std::string text, float scale, float lineHeight = 1.2, float textSpacing = 1, coord_t bdims = { 0,0 });
		GUI* guiPtr;
	};

	struct FontManager
	{
		FontManager(GUI* guiPtr);
		~FontManager();

		Font* addFont(std::string fontName, std::string fontPath, int size, float weight = 0.f, bool outline = 0);

		std::map<std::string, Font*> fonts;

		Font* defaultFont;
		FT_Library ft;
		GUI* guiPtr;
	};
}
