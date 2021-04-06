#include <GL/glew.h>
#include "FontManager.h"
#include "Utilities.h"
#include "GUI.h"
#include <sstream>

#include FT_OUTLINE_H
#include FT_BITMAP_H
#include FT_STROKER_H
#include FT_FREETYPE_H

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

using gui::FontManager;
using gui::Font;

FontManager::FontManager(GUI* guiPtr)
	:guiPtr(guiPtr)
{
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	defaultFont = nullptr;
}

FontManager::~FontManager()
{
	FT_Done_FreeType(ft);
}

Font* FontManager::addFont(std::string fontName, std::string fontPath, int size, float weight, bool outline)
{
	Font* f = new Font(&ft, fontPath, fontName, size, guiPtr, weight, outline);
	fonts[fontName] = f;
	return f;
}

Font::Font(FT_Library* ft, std::string fontPath, std::string fontName, int size, GUI* guiPtr, float weight, bool outline)
	:fontPath(fontPath), size(size), guiPtr(guiPtr), fontName(fontName)
{
	glEnable(GL_TEXTURE_2D);

	FT_Face face;
	FT_Face itaicFace = 0;

	if (FT_New_Memory_Face(*ft, (const FT_Byte*)guiPtr->filesys[fontPath].data, guiPtr->filesys[fontPath].size, 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	std::string fontExtension = fontPath.substr(fontPath.find_last_of('.'));
	std::string italicFontPath = fontPath.substr(0, fontPath.find_last_of('.')) + "Italic" + fontExtension;

	int styles = 2;

	if (guiPtr->filesys.find(italicFontPath) != guiPtr->filesys.end())
	{
		std::cout << "Found Italic Font for " << fontName << " (" << italicFontPath << ")" << std::endl;
		if (FT_New_Memory_Face(*ft, (const FT_Byte*)guiPtr->filesys[italicFontPath].data, guiPtr->filesys[italicFontPath].size, 0, &itaicFace))
			std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		FT_Set_Pixel_Sizes(itaicFace, 0, size);

		styles = 3;
	}
	else
	{
		std::cout << "No italic font: " << italicFontPath << std::endl;
	}


	FT_Stroker stroker;
	FT_Stroker_New(*ft, &stroker);
	FT_Set_Pixel_Sizes(face, 0, size);
	int numGlyphs = std::min((int)face->num_glyphs, 128);

	fontAtlas = new FontAtlas();

	std::cout << fontName << ": " << face->num_faces << " embedded faces, " << numGlyphs << " glyphs." << std::endl;


	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
	for (int c = 0; c < numGlyphs; c++)
	{
		for (int style = 0; style < styles; style++)
		{
			FT_Face lFace = face;

			if (style == 2)
				lFace = itaicFace;

			if (FT_Load_Char(lFace, c, FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP))
				continue;

			FT_Glyph glyphD;
			if (FT_Get_Glyph(lFace->glyph, &glyphD) != 0)
				continue;

			FT_Stroker_Set(stroker, weight + ((style == 1) * .33) * 64.f, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);

			if (!outline)
				FT_Glyph_StrokeBorder(&glyphD, stroker, false, true);
			else
				FT_Glyph_Stroke(&glyphD, stroker, true);

			FT_Glyph_To_Bitmap(&glyphD, FT_RENDER_MODE_NORMAL, 0, 1);

			FT_BitmapGlyph btGlyph = (FT_BitmapGlyph)glyphD;

			fontAtlas->precalcGlyph(btGlyph->bitmap.width, btGlyph->bitmap.rows);
			char* glyphCopy = new char[btGlyph->bitmap.width * btGlyph->bitmap.rows];
			memcpy(glyphCopy, btGlyph->bitmap.buffer, btGlyph->bitmap.width * btGlyph->bitmap.rows);

			Character character = {
				{btGlyph->bitmap.width, btGlyph->bitmap.rows},
				{btGlyph->left, btGlyph->top},
				lFace->glyph->advance.x,
				FontAtlasEntry(),
				glyphCopy
			};
			characters.insert(std::pair<uint32_t, Character>(c + style * 0x4000, character));
		}
	}

	fontAtlas->finishPrecalc();
	fontAtlas->realloc();

	for (auto& i : characters)
	{
		i.second.ae = fontAtlas->addGlyph((char*)i.second.glyphData, i.second.Size[0], i.second.Size[1]);
		delete[] i.second.glyphData;
	}

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_ALPHA,
		fontAtlas->width,
		fontAtlas->height,
		0,
		GL_ALPHA,
		GL_UNSIGNED_BYTE,
		fontAtlas->data
	);
	fontAtlas->textureID = texture;

	fontAtlas->writeAtlas("atlas/" + fontName + ".png");

	fontAtlas->free();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	std::cout << "Loaded typeface: " << fontPath << " (" << size << " px)" << std::endl;
	FT_Done_Face(face);
	FT_Done_Face(itaicFace);
	FT_Stroker_Done(stroker);
	/*glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);*/
	glDisable(GL_TEXTURE_2D);
}

void Font::layoutText(std::string text, float scale, float textSpacing, gui::coord_t bdims, std::vector<LineInfo>* layout)
{
	if (!layout)
		return;

	float x = 0;

	std::string lineText;

	for (int c = 0; c < text.length(); ++c)
	{
		if (text[c] == '\n')
		{
			layout->push_back({ x, lineText });
			lineText = "";
			x = 0;
			continue;
		}

		lineText += text[c];

		if (c < text.length() - 1)
		{
			if (text[c] == '*' && text[c + 1] == '*')
			{
				c++;
				lineText += text[c];
				continue;
			}
			if (text[c] == '#' && text[c + 1] == '#')
			{
				c++;
				lineText += text[c];
				continue;
			}
			if (text[c] == '_' && text[c + 1] == '_')
			{
				c++;
				lineText += text[c];
				continue;
			}
		}

		Character ch = characters[text[c]];
		float advance = (float)(ch.Advance >> 6) * scale * textSpacing;

		x += std::roundf(advance);

		float w = ch.Size[0] * scale;

		if (bdims.x != 0)
		{
			if (x + w + ch.Bearing[0] * scale + 4 > bdims.x)
			{
				layout->push_back({ x, lineText });
				lineText = "";
				x = 0;
			}
		}
	}
	layout->push_back({ x, lineText });
}

void Font::renderText(std::string text_, float x, float y, float scale, bool bold, bool italic, int justify, float lineHeight, float textSpacing, coord_t bdims)
{
	/*glBegin(GL_LINES);
	glVertex2f(x, y-size);
	glVertex2f(x, y+100);
	glEnd();*/

	//glDisable(GL_MULTISAMPLE);
	//SplitTimer::Timer t;
	//t.reset();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, fontAtlas->textureID);
	glBegin(GL_QUADS);

	std::vector<LineInfo> layout;


	layoutText(text_, scale, textSpacing, bdims, &layout);

	int currentLine = 0;
	float initialX = x;
	float initialY = y;
	int italicState = 0;
	int boldState = 0;
	int underlineState = 0;
	bool underlineSwitched = 0;

	std::vector<line_t> underline;

	float lastAdvance = 0;

	for (int q = 0; q < layout.size(); ++q)
	{
		float width = layout[q].width;
		std::string text = layout[q].text;
		currentLine = q;
		x = initialX;

		if (justify == 1)
			x -= (width - bdims.x) * .5;
		if (justify == 2)
			x -= width - bdims.x;


		y = initialY + q * size * lineHeight;

		for (int c = 0; c < text.length(); ++c)
		{
			if (c < text.length() - 1)
			{
				if (c < text.length() - 2)
				{
					if (text[c + 1] == '_' && text[c + 2] == '_')
					{
						underlineSwitched = true;
					}
				}
				if (text[c] == '*' && text[c + 1] == '*')
				{
					boldState = !boldState;
					c++;
					continue;
				}

				if (text[c] == '#' && text[c + 1] == '#')
				{
					italicState = !italicState;
					c++;
					continue;
				}


				if (text[c] == '_' && text[c + 1] == '_')
				{
					underlineState = !underlineState;
					underlineSwitched = true;
					c++;
					continue;
				}
			}

			int faceIndex = (boldState || bold) + ((italicState || italic) * 2);

			Character ch = characters[text[c] + 0x4000 * faceIndex];

			float xpos = x + ch.Bearing[0] * scale;
			float ypos = y - (ch.Bearing[1]) * scale;

			float w = ch.Size[0] * scale;
			float h = ch.Size[1] * scale;

			float advance = (float)(ch.Advance >> 6) * scale * textSpacing;

			float italicSkew = w * .2 * (italicState || italic);

			italicSkew = 0;

			float vertices[6][4] = {
				{ xpos + italicSkew,     ypos ,    ch.ae.x[0], ch.ae.y[0] },
				{ xpos + italicSkew + w, ypos,     ch.ae.x[1], ch.ae.y[0] },
				{ xpos + w, ypos + h, ch.ae.x[1], ch.ae.y[1] },
				{ xpos,     ypos + h, ch.ae.x[0], ch.ae.y[1] }
			};

			if (underlineState)
			{
				underline.push_back({ {xpos - lastAdvance * .51f * !underlineSwitched, y + 2.f}, {xpos + w + advance * .51f * !underlineSwitched, y + 2.f} });
				underlineSwitched = false;
			}

			for (int i = 0; i < 4; i++)
			{
				glTexCoord2f(vertices[i][2], vertices[i][3]);
				glVertex2f(std::roundf(vertices[i][0]), std::roundf(vertices[i][1]));
			}

			x += std::roundf(advance);
			lastAdvance = advance;
		}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
	//glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glLineWidth(1);

	if (underline.size())
	{
		glBegin(GL_LINES);
		for (auto& i : underline)
		{
			glVertex2f(i.first.x, i.first.y);
			glVertex2f(i.second.x, i.second.y);
		}
		glEnd();
	}
	//t.split("Finished rendering text: " + text.substr(0, 10), SplitTimer::defaultSplitPrint);
	//glEnable(GL_MULTISAMPLE);
}

void gui::Font::renderTextUnicode(std::wstring text, float x, float y, float scale)
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, fontAtlas->textureID);
	glBegin(GL_QUADS);

	for (auto& c : text)
	{
		Character ch = characters[c];

		float xpos = x + ch.Bearing[0] * scale;
		float ypos = y - (ch.Bearing[1]) * scale;

		float w = ch.Size[0] * scale;
		float h = ch.Size[1] * scale;
		// Update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos ,    ch.ae.x[0], ch.ae.y[0] },
			{ xpos + w, ypos,     ch.ae.x[1], ch.ae.y[0] },
			{ xpos + w, ypos + h, ch.ae.x[1], ch.ae.y[1] },
			{ xpos,     ypos + h, ch.ae.x[0], ch.ae.y[1] }
		};
		// Render glyph texture over quad

		for (int i = 0; i < 4; i++)
		{
			glTexCoord2f(vertices[i][2], vertices[i][3]);
			glVertex2f(vertices[i][0], vertices[i][1]);
		}


		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
	//glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

gui::coord_t Font::textMetrics(std::string text, float scale, float lineHeight, float textSpacing, coord_t bdims)
{
	std::vector<LineInfo> layout;
	layoutText(text, scale, textSpacing, bdims, &layout);

	float maxWidth = 0;

	for (auto& i : layout)
	{
		if (i.width > maxWidth)
			maxWidth = i.width;
	}

	return {maxWidth, layout.size() * size * lineHeight};
}

inline int getIndex(int x, int y, int width)
{
	return y * width + x;
}

gui::FontAtlas::FontAtlas()
{
	width = 1024;
	height = 1024;
	margin = 1;

	data = new char[width * height];
	memset(data, 0, width * height);
}

gui::FontAtlasEntry gui::FontAtlas::addGlyph(char* buf, int w, int h)
{
	if (cursorX + w + 2 * margin > width)
	{
		cursorX = 0;
		cursorY += tallestGlyphHeight + 2 * margin;
	}

	gui::FontAtlasEntry ret;

	cursorX += margin;
	cursorY += margin;

	ret.x[0] = (float)(cursorX) / width;
	ret.y[0] = (float)(cursorY) / height;
	ret.x[1] = (float)(cursorX + w) / width;
	ret.y[1] = (float)(cursorY + h) / height;

	for (int i = 0; i < w; ++i)
	{
		for (int j = 0; j < h; ++j)
		{
			data[getIndex(i + cursorX, j + cursorY, width) % (width * height)] = buf[getIndex(i, j, w)];
		}
	}

	cursorX += margin;
	cursorY -= margin;

	if (h > tallestGlyphHeight) tallestGlyphHeight = h;

	maxY = cursorY + tallestGlyphHeight + margin * 2;

	cursorX += w;
	return ret;
}

void gui::FontAtlas::precalcGlyph(int w, int h)
{
	if (cursorX + w + 2 * margin > width)
	{
		cursorX = 0;
		cursorY += tallestGlyphHeight + 2 * margin;
	}

	cursorX += margin * 2;

	if (h > tallestGlyphHeight) tallestGlyphHeight = h;

	maxY = cursorY + tallestGlyphHeight + margin * 2;

	cursorX += w;
}

void gui::FontAtlas::finishPrecalc()
{
	width = width;
	height = maxY;
	cursorX = 0;
	cursorY = 0;
	tallestGlyphHeight = 0;
}

void gui::FontAtlas::writeAtlas(std::string fname)
{
	std::filesystem::path pa = __FILE__;
	pa = pa.parent_path() /= fname;
	fname = pa.string();

	stbi_write_png(fname.c_str(), width, height, 1, data, width);
}

void gui::FontAtlas::realloc()
{
	delete[] data;
	data = new char[width * height];
	memset(data, 0, width * height);
}

void gui::FontAtlas::free()
{
	delete[] data;
}
