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

#include "collision/CollisionCircle.h"
#include "collision/CollisionRect2D.h"
#include "collision/StandardIntersectionHandlers.h"

static bool intersectSweptLineSegments(fixed16_16 x1Start, fixed16_16 x1End,
                                       fixed16_16 x2Start, fixed16_16 x2End,
                                       fixed16_16 bottom1Start, fixed16_16 top1Start, fixed16_16 bottom1End, fixed16_16 top1End,
                                       fixed16_16 bottom2Start, fixed16_16 top2Start, fixed16_16 bottom2End, fixed16_16 top2End,
                                       fixed16_16 * outTime) {
	fixed16_16 time;
	fixed16_16 bottom1, top1, bottom2, top2;
	
	time = xdiv(x1Start - x2Start, x2End - x2Start - x1End + x1Start);
	if (time > 0x10000 || time < 0x00000) {
		return false;
	}
	bottom1 = bottom1Start + xmul(bottom1End - bottom1Start, time);
	top1    = top1Start    + xmul(top1End    - top1Start,    time);
	bottom2 = bottom2Start + xmul(bottom2End - bottom2Start, time);
	top2    = top2Start    + xmul(top2End    - top2Start,    time);
	if (top1 > bottom2 && bottom1 < top2) {
		*outTime = time;
		return true;
	}
	return false;
}

/*
static bool intersectSweptQuads(fixed16_16 x1Start, fixed16_16 x1End,
                                fixed16_16 x2Start, fixed16_16 x2End,
                                fixed16_16 bottom1Start, fixed16_16 top1Start, fixed16_16 bottom1End, fixed16_16 top1End,
                                fixed16_16 bottom2Start, fixed16_16 top2Start, fixed16_16 bottom2End, fixed16_16 top2End,
                                fixed16_16 back1Start, fixed16_16 front1Start, fixed16_16 back1End, fixed16_16 front1End,
                                fixed16_16 back2Start, fixed16_16 front2Start, fixed16_16 back2End, fixed16_16 front2End,
                                fixed16_16 * outTime) {
	fixed16_16 time;
	fixed16_16 bottom1, top1, bottom2, top2, back1, front1, back2, front2;
	
	time = xdiv(x1Start - x2Start, x2End - x2Start - x1End + x1Start);
	if (time > 0x10000) {
		return false;
	}
	bottom1 = bottom1Start + xmul(bottom1End - bottom1Start, time);
	top1    = top1Start    + xmul(top1End    - top1Start,    time);
	bottom2 = bottom2Start + xmul(bottom2End - bottom2Start, time);
	top2    = top2Start    + xmul(top2End    - top2Start,    time);
	back1   = back1Start   + xmul(back1End   - back1Start,   time);
	front1  = front1Start  + xmul(front1End  - front1Start,  time);
	back2   = back2Start   + xmul(back2End   - back2Start,   time);
	front2  = front2Start  + xmul(front2End  - front2Start,  time);
	if (top1 > bottom2 && bottom1 < top2 && front1 > back2 && back1 < front2) {
		if (time < 0) {
			time = 0;
		}
		*outTime = time;
		return true;
	}
	return false;
}
*/

static bool intersectSweptPoints(fixed16_16 x1Start, fixed16_16 x1End,
                                 fixed16_16 y1Start, fixed16_16 y1End,
                                 fixed16_16 x2Start, fixed16_16 x2End,
                                 fixed16_16 y2Start, fixed16_16 y2End,
                                 fixed16_16 * outTime) {
	fixed16_16 time1, time2;
	
	time1 = xdiv(x1Start - x2Start, x2End - x2Start - x1End + x1Start);
	time2 = xdiv(y1Start - y2Start, y2End - y2Start - y1End + y1Start);
	if (time1 > 0x10000 || time1 < 0x00000 || time2 > 0x10000 || time2 < 0x00000 || time1 != time2) {
		return false;
	}
	*outTime = time1;
	return true;
}

bool intersectionHandler_rect2D_rect2D(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	fixed16_16 time;
	CollisionRect2D * rect1 = (CollisionRect2D *) object1, * rect2 = (CollisionRect2D *) object2;
	
	// rect1 right vs. rect2 left
	if (intersectSweptLineSegments(rect1->lastPosition.x + rect1->lastSize.x, rect1->position.x + rect1->size.x,
	                               rect2->lastPosition.x, rect2->position.x,
	                               rect1->lastPosition.y, rect1->lastPosition.y + rect1->lastSize.y, rect1->position.y, rect1->position.y + rect1->size.y,
	                               rect2->lastPosition.y, rect2->lastPosition.y + rect2->lastSize.y, rect2->position.y, rect2->position.y + rect2->size.y,
	                               &time)) {
		if (outTime != NULL) {
			*outTime = time;
		}
		if (outNormal != NULL) {
			*outNormal = VECTOR3x_LEFT;
		}
		return true;
	}
	
	// rect1 left vs. rect2 right
	if (intersectSweptLineSegments(rect1->lastPosition.x, rect1->position.x,
	                               rect2->lastPosition.x + rect2->lastSize.x, rect2->position.x + rect2->size.x,
	                               rect1->lastPosition.y, rect1->lastPosition.y + rect1->lastSize.y, rect1->position.y, rect1->position.y + rect1->size.y,
	                               rect2->lastPosition.y, rect2->lastPosition.y + rect2->lastSize.y, rect2->position.y, rect2->position.y + rect2->size.y,
	                               &time)) {
		if (outTime != NULL) {
			*outTime = time;
		}
		if (outNormal != NULL) {
			*outNormal = VECTOR3x_RIGHT;
		}
		return true;
	}
	
	// rect1 top vs. rect2 bottom
	if (intersectSweptLineSegments(rect1->lastPosition.y + rect1->lastSize.y, rect1->position.y + rect1->size.y,
	                               rect2->lastPosition.y, rect2->position.y,
	                               rect1->lastPosition.x, rect1->lastPosition.x + rect1->lastSize.x, rect1->position.x, rect1->position.x + rect1->size.x,
	                               rect2->lastPosition.x, rect2->lastPosition.x + rect2->lastSize.x, rect2->position.x, rect2->position.x + rect2->size.x,
	                               &time)) {
		if (outTime != NULL) {
			*outTime = time;
		}
		if (outNormal != NULL) {
			*outNormal = VECTOR3x_DOWN;
		}
		return true;
	}
	
	// rect1 bottom vs. rect2 top
	if (intersectSweptLineSegments(rect1->lastPosition.y, rect1->position.y,
	                               rect2->lastPosition.y + rect2->lastSize.y, rect2->position.y + rect2->size.y,
	                               rect1->lastPosition.x, rect1->lastPosition.x + rect1->lastSize.x, rect1->position.x, rect1->position.x + rect1->size.x,
	                               rect2->lastPosition.x, rect2->lastPosition.x + rect2->lastSize.x, rect2->position.x, rect2->position.x + rect2->size.x,
	                               &time)) {
		if (outTime != NULL) {
			*outTime = time;
		}
		if (outNormal != NULL) {
			*outNormal = VECTOR3x_UP;
		}
		return true;
	}
	
	// rect1 top right corner vs. rect2 bottom left corner
	if (intersectSweptPoints(rect1->lastPosition.x + rect1->lastSize.x, rect1->position.x + rect1->size.x,
	                         rect1->lastPosition.y + rect1->lastSize.y, rect1->position.y + rect1->size.y,
	                         rect2->lastPosition.x, rect2->position.x,
	                         rect2->lastPosition.y, rect2->position.y,
	                         &time)) {
		if (outTime != NULL) {
			*outTime = time;
		}
		if (outNormal != NULL) {
			*outNormal = VECTOR3x_LEFT;
		}
		return true;
	}
	
	// rect1 bottom right corner vs. rect2 top left corner
	if (intersectSweptPoints(rect1->lastPosition.x + rect1->lastSize.x, rect1->position.x + rect1->size.x,
	                         rect1->lastPosition.y, rect1->position.y,
	                         rect2->lastPosition.x, rect2->position.x,
	                         rect2->lastPosition.y + rect2->lastSize.y, rect2->position.y + rect2->size.y,
	                         &time)) {
		if (outTime != NULL) {
			*outTime = time;
		}
		if (outNormal != NULL) {
			*outNormal = VECTOR3x_LEFT;
		}
		return true;
	}
	
	// rect1 top left corner vs. rect2 bottom right corner
	if (intersectSweptPoints(rect1->lastPosition.x, rect1->position.x,
	                         rect1->lastPosition.y + rect1->lastSize.y, rect1->position.y + rect1->size.y,
	                         rect2->lastPosition.x + rect2->lastSize.x, rect2->position.x + rect2->size.x,
	                         rect2->lastPosition.y, rect2->position.y,
	                         &time)) {
		if (outTime != NULL) {
			*outTime = time;
		}
		if (outNormal != NULL) {
			*outNormal = VECTOR3x_RIGHT;
		}
		return true;
	}
	
	// rect1 bottom left corner vs. rect2 top right corner
	if (intersectSweptPoints(rect1->lastPosition.x, rect1->position.x,
	                         rect1->lastPosition.y, rect1->position.y,
	                         rect2->lastPosition.x + rect2->lastSize.x, rect2->position.x + rect2->size.x,
	                         rect2->lastPosition.y + rect2->lastSize.y, rect2->position.y + rect2->size.y,
	                         &time)) {
		if (outTime != NULL) {
			*outTime = time;
		}
		if (outNormal != NULL) {
			*outNormal = VECTOR3x_RIGHT;
		}
		return true;
	}
	
	return false;
}

bool intersectionHandler_rect2D_circle(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}

bool intersectionHandler_rect2D_line2D(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}

bool intersectionHandler_rect2D_polygon(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}

bool intersectionHandler_circle_circle(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	CollisionCircle * circle1 = (CollisionCircle *) object1, * circle2 = (CollisionCircle *) object2;
	fixed16_16 time;
	fixed16_16 x1Start, y1Start, x1End, y1End;
	fixed16_16 x2Start, y2Start, x2End, y2End;
	fixed16_16 radius1, radius2;
	
	x1Start = circle1->lastPosition.x;
	x1End = circle1->lastPosition.y;
	y1Start = circle1->position.x;
	y1End = circle1->position.y;
	x2Start = circle2->lastPosition.x;
	x2End = circle2->lastPosition.y;
	y2Start = circle2->position.x;
	y2End = circle2->position.y;
	radius1 = circle1->radius;
	radius2 = circle2->radius;
	
	time = /* some mess of algebra on above variables */ -1;
	if (time > 0x10000 || time < 0x00000) {
		fixed16_16 x1, y1, x2, y2;
		Vector3x normal;
		
		x1 = x1Start + xmul(x1End - x1Start, time);
		y1 = y1Start + xmul(y1End - y1Start, time);
		x2 = x2Start + xmul(x2End - x2Start, time);
		y2 = y2Start + xmul(y2End - y2Start, time);
		normal.x = x1 - x2;
		normal.y = y1 - y2;
		normal.z = 0x00000;
		Vector3x_normalize(&normal);
		if (outNormal != NULL) {
			*outNormal = normal;
		}
		if (outTime != NULL) {
			*outTime = time;
		}
		return true;
	}
	return false;
}

bool intersectionHandler_circle_line2D(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}

bool intersectionHandler_circle_polygon(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}

bool intersectionHandler_line2D_line2D(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}

bool intersectionHandler_line2D_polygon(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}

bool intersectionHandler_polygon_polygon(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}

bool intersectionHandler_box3D_box3D(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}

bool intersectionHandler_box3D_sphere(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}

bool intersectionHandler_box3D_line3D(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}

bool intersectionHandler_box3D_cylinder(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}

bool intersectionHandler_box3D_capsule(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}

bool intersectionHandler_box3D_trimesh(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}

bool intersectionHandler_sphere_sphere(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}

bool intersectionHandler_sphere_line3D(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}

bool intersectionHandler_sphere_cylinder(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}

bool intersectionHandler_sphere_capsule(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}

bool intersectionHandler_sphere_trimesh(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}

bool intersectionHandler_line3D_line3D(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}

bool intersectionHandler_line3D_cylinder(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}

bool intersectionHandler_line3D_capsule(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}

bool intersectionHandler_line3D_trimesh(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}

bool intersectionHandler_cylinder_cylinder(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}

bool intersectionHandler_cylinder_capsule(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}

bool intersectionHandler_cylinder_trimesh(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}

bool intersectionHandler_capsule_capsule(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}

bool intersectionHandler_capsule_trimesh(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}

bool intersectionHandler_trimesh_trimesh(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal) {
	return false;
}
