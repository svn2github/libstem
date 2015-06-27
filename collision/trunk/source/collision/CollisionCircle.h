/*
  Copyright (c) 2015 Alex Diener
  
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

#ifndef __CollisionCircle_H__
#define __CollisionCircle_H__
#ifdef __cplusplus
extern "C" {
#endif

typedef struct CollisionCircle CollisionCircle;

#include "collision/CollisionObject.h"
#include "gamemath/Vector2x.h"

#define CollisionCircle_structContents(self_type) \
	CollisionObject_structContents(self_type) \
	\
	bool concave; \
	Vector2x position; \
	Vector2x lastPosition; \
	fixed16_16 radius; \
	fixed16_16 lastRadius;

stemobject_struct_definition(CollisionCircle)

// position is the center of the circle.
// radius extends the circle outward from position.
// If concave is true, this circle will be considered an enclosed empty space, and collisions will be detected with its inner edges
// for objects moving outward. Objects moving from outside a concave circle into it will not collide with its edges.
// If concave is false, this circle will be considered solid, and collisions will be detected with its outer edges for
// objects moving inward. Objects moving out of convex circle from inside it will not collide with its edges.
CollisionCircle * CollisionCircle_create(void * owner, CollisionCallback collisionCallback, Vector2x position, fixed16_16 radius, bool concave);
bool CollisionCircle_init(CollisionCircle * self, void * owner, CollisionCallback collisionCallback, Vector2x position, fixed16_16 radius, bool concave);
void CollisionCircle_dispose(CollisionCircle * self);

void CollisionCircle_updatePosition(CollisionCircle * self, Vector2x newPosition);
void CollisionCircle_updateRadius(CollisionCircle * self, fixed16_16 newRadius);
void CollisionCircle_interpolate(CollisionCircle * self, fixed16_16 amount);

#ifdef __cplusplus
}
#endif
#endif