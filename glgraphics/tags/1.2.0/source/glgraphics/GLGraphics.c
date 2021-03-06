/*
  Copyright (c) 2013 Alex Diener
  
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

#include "glgraphics/GLGraphics.h"
#include <stdbool.h>
#ifndef GLGRAPHICS_NO_GLEW
#include "glgraphics/GLIncludes.h"
#include <stdio.h>
#endif

static enum GLAPIVersion openGLAPIVersion = GL_API_VERSION_DESKTOP_1;

void GLGraphics_init(enum GLAPIVersion apiVersion) {
#ifndef GLGRAPHICS_NO_GLEW
	GLenum glewStatus;
	
	glewStatus = glewInit();
	if (glewStatus != GLEW_OK) {
		fprintf(stderr, "Warning: glewInit() failed: %s\n", glewGetErrorString(glewStatus));
	}
#endif
	openGLAPIVersion = apiVersion;
}

enum GLAPIVersion GLGraphics_getOpenGLAPIVersion() {
	return openGLAPIVersion;
}
