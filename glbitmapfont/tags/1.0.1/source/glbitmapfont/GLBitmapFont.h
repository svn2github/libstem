/*
  Copyright (c) 2010 Alex Diener
  
  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.
  
  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:
  
  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
  
  Alex Diener adiener@sacredsoftware.net
*/

#ifndef __GL_BITMAP_FONT_H__
#define __GL_BITMAP_FONT_H__

#include "glfont/GLFont.h"
#include "gltexture/GLTexture.h"
#include "serialization/DeserializationContext.h"
#include "serialization/SerializationContext.h"

typedef struct GLBitmapFont GLBitmapFont;

// TODO: Remove in favor of arbitrary set of characters per font
#define GLBITMAPFONT_PRINTABLE_MIN ' '
#define GLBITMAPFONT_PRINTABLE_MAX '~'
#define GLBITMAPFONT_NUM_CHARS 95

#define GLBITMAPFONT_SERIALIZATION_FORMAT_VERSION 1

struct GLBitmapFont_charEntry {
	// Number of total ems advanced after drawing this glyph
	float advance;
	// Number of ems to the right of the current position to start drawing glyph
	float glyphOffset;
	// Glyph's drawn width in ems
	float glyphWidth;
	// Glyph's texture coordinates
	float textureLeft;
	float textureRight;
	float textureBottom;
	float textureTop;
	// Number of items in kernChars
	size_t kernCharCount;
	struct GLBitmapFont_kernEntry {
		// If this character is preceded by previous, offset is applied to both preadvance and advance.
		// previous must be unique within the kernChars list, and is only meaningful if it is one of the 95
		// printable characters represented by GLBitmapFont (' ' through '~').
		char previous;
		float offset;
	} * kernChars;
};

#define GLBitmapFont_structContents \
	GLFont_structContents \
	\
	char * textureName; \
	bool textureNameOwned; \
	GLTexture * texture; \
	bool textureOwned; \
	struct GLBitmapFont_charEntry characters[GLBITMAPFONT_NUM_CHARS]; \
	\
	void (* setTexture)(compat_type(GLBitmapFont *) selfPtr, compat_type(GLTexture *) texturePtr, bool takeOwnership);

struct GLBitmapFont {
	GLBitmapFont_structContents
};

GLBitmapFont * GLBitmapFont_create(struct GLBitmapFont_charEntry characters[GLBITMAPFONT_NUM_CHARS]);
void GLBitmapFont_init(compat_type(GLBitmapFont *) selfPtr, struct GLBitmapFont_charEntry characters[GLBITMAPFONT_NUM_CHARS]);
void GLBitmapFont_dispose(compat_type(GLBitmapFont *) selfPtr);

GLBitmapFont * GLBitmapFont_deserialize(compat_type(DeserializationContext *) deserializationContext);
bool GLBitmapFont_loadSerializedData(compat_type(GLBitmapFont *) selfPtr, compat_type(DeserializationContext *) deserializationContext);
void GLBitmapFont_serialize(compat_type(GLBitmapFont *) selfPtr, compat_type(SerializationContext *) serializationContext);

void GLBitmapFont_setTexture(compat_type(GLBitmapFont *) selfPtr, compat_type(GLTexture *) texturePtr, bool takeOwnership);

float GLBitmapFont_measureString(compat_type(GLBitmapFont *) selfPtr, const char * string, size_t length);
size_t GLBitmapFont_indexAtWidth(compat_type(GLBitmapFont *) selfPtr, const char * string, size_t length, float emWidth, bool * outLeadingEdge);
// string must not be modified while GLBitmapFont_drawString is executing
// Currently cannot correctly draw strings longer than 16383 characters
void GLBitmapFont_drawString(compat_type(GLBitmapFont *) selfPtr, const char * string, size_t length, float emHeight, float offsetX, float offsetY, float offsetZ);

#endif
