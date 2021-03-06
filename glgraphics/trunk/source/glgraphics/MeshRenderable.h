/*
  Copyright (c) 2018 Alex Diener
  
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
  
  Alex Diener alex@ludobloom.com
*/

#ifndef __MeshRenderable_H__
#define __MeshRenderable_H__
#ifdef __cplusplus
extern "C" {
#endif

typedef struct MeshRenderable MeshRenderable;

#include "gamemath/Matrix4x4f.h"
#include "glgraphics/AnimationState.h"
#include "glgraphics/Material.h"
#include "glgraphics/Renderable.h"
#include "glgraphics/VertexBuffer.h"

#define MeshRenderable_structContents(self_type) \
	Renderable_structContents(self_type) \
	\
	VertexBuffer * vertexBuffer; \
	Material * material; \
	AnimationState * animationState; \
	Matrix4x4f transform;

stemobject_struct_definition(MeshRenderable)

// If animationState is NULL, vertexBuffer must be in PTNXC format. If animationState is non-NULL, vertexBuffer must be in PTNXCBW format.
MeshRenderable * MeshRenderable_create(VertexBuffer * vertexBuffer, Material * material, AnimationState * animationState, Matrix4x4f transform);
bool MeshRenderable_init(MeshRenderable * self, VertexBuffer * vertexBuffer, Material * material, AnimationState * animationState, Matrix4x4f transform);
void MeshRenderable_dispose(MeshRenderable * self);
unsigned int MeshRenderable_getTextureBindID(MeshRenderable * self);
void MeshRenderable_getVertices(MeshRenderable * self, void * outVertices, GLuint * outIndexes, unsigned int * ioVertexCount, unsigned int * ioIndexCount);

#ifdef __cplusplus
}
#endif
#endif
