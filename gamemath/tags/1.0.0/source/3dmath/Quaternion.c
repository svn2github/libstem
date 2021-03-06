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

#include "3dmath/Quaternion.h"

#include <math.h>
#include <stdlib.h>

#include "3dmath/Matrix.h"
#include "3dmath/Vector.h"

void Quaternion_loadIdentity(Quaternion * quaternion) {
	quaternion->x = 0.0f;
	quaternion->y = 0.0f;
	quaternion->z = 0.0f;
	quaternion->w = 1.0f;
}

Quaternion Quaternion_identity() {
	Quaternion quaternion;
	
	Quaternion_loadIdentity(&quaternion);
	return quaternion;
}

Quaternion Quaternion_init(float x, float y, float z, float w) {
	Quaternion quaternion;
	
	quaternion.x = x;
	quaternion.y = y;
	quaternion.z = z;
	quaternion.w = w;
	return quaternion;
}

Quaternion Quaternion_fromVector(Vector3 vector) {
	Quaternion quaternion;
	
	quaternion.x = vector.x;
	quaternion.y = vector.y;
	quaternion.z = vector.z;
	quaternion.w = 0.0f;
	return quaternion;
}

Vector3 Quaternion_toVector(Quaternion quaternion) {
	Vector3 vector;
	
	vector.x = quaternion.x;
	vector.y = quaternion.y;
	vector.z = quaternion.z;
	
	return vector;
}

Quaternion Quaternion_fromAxisAngle(Vector3 axis, float angle) {
	Quaternion quaternion;
	float sinAngle;
	
	angle *= 0.5f;
	Vector3_normalize(&axis);
	sinAngle = sin(angle);
	quaternion.x = axis.x * sinAngle;
	quaternion.y = axis.y * sinAngle;
	quaternion.z = axis.z * sinAngle;
	quaternion.w = cos(angle);
	
	return quaternion;
}

void Quaternion_toAxisAngle(Quaternion quaternion, Vector3 * axis, float * angle) {
	float sinAngle;
	
	Quaternion_normalize(&quaternion);
	sinAngle = sqrtf(1.0f - quaternion.w * quaternion.w);
	if (fabsf(sinAngle) < 0.0005f) {
		sinAngle = 1.0f;
	}
	
	if (axis != NULL) {
		axis->x = quaternion.x / sinAngle;
		axis->y = quaternion.y / sinAngle;
		axis->z = quaternion.z / sinAngle;
	}
	
	if (angle != NULL) {
		*angle = acos(quaternion.w) * 2.0f;
	}
}

Matrix Quaternion_toMatrix(Quaternion quaternion) {
	Matrix matrix;
	
	matrix.m[0]  = 1.0f - 2.0f * (quaternion.y * quaternion.y + quaternion.z * quaternion.z);
	matrix.m[1]  =        2.0f * (quaternion.x * quaternion.y + quaternion.z * quaternion.w);
	matrix.m[2]  =        2.0f * (quaternion.x * quaternion.z - quaternion.y * quaternion.w);
	matrix.m[3]  = 0.0f;
	matrix.m[4]  =        2.0f * (quaternion.x * quaternion.y - quaternion.z * quaternion.w);
	matrix.m[5]  = 1.0f - 2.0f * (quaternion.x * quaternion.x + quaternion.z * quaternion.z);
	matrix.m[6]  =        2.0f * (quaternion.y * quaternion.z + quaternion.x * quaternion.w);
	matrix.m[7]  = 0.0f;
	matrix.m[8]  =        2.0f * (quaternion.x * quaternion.z + quaternion.y * quaternion.w);
	matrix.m[9]  =        2.0f * (quaternion.y * quaternion.z - quaternion.x * quaternion.w);
	matrix.m[10] = 1.0f - 2.0f * (quaternion.x * quaternion.x + quaternion.y * quaternion.y);
	matrix.m[11] = 0.0f;
	matrix.m[12] = 0.0f;
	matrix.m[13] = 0.0f;
	matrix.m[14] = 0.0f;
	matrix.m[15] = 1.0f;
	
	return matrix;
}

void Quaternion_normalize(Quaternion * quaternion) {
	float magnitude;
	
	magnitude = sqrtf(quaternion->x * quaternion->x +
	                  quaternion->y * quaternion->y +
	                  quaternion->z * quaternion->z +
	                  quaternion->w * quaternion->w);
	quaternion->x /= magnitude;
	quaternion->y /= magnitude;
	quaternion->z /= magnitude;
	quaternion->w /= magnitude;
}

Quaternion Quaternion_normalized(Quaternion quaternion) {
	Quaternion_normalize(&quaternion);
	return quaternion;
}

void Quaternion_multiply(Quaternion * quaternion1, Quaternion quaternion2) {
	Vector3 vector1, vector2, cross;
	float angle;
	
	vector1 = Quaternion_toVector(*quaternion1);
	vector2 = Quaternion_toVector(quaternion2);
	angle = quaternion1->w * quaternion2.w - Vector3_dot(vector1, vector2);
	
	cross = Vector3_cross(vector1, vector2);
	vector1.x *= quaternion2.w;
	vector1.y *= quaternion2.w;
	vector1.z *= quaternion2.w;
	vector2.x *= quaternion1->w;
	vector2.y *= quaternion1->w;
	vector2.z *= quaternion1->w;
	
	quaternion1->x = vector1.x + vector2.x + cross.x;
	quaternion1->y = vector1.y + vector2.y + cross.y;
	quaternion1->z = vector1.z + vector2.z + cross.z;
	quaternion1->w = angle;
}

Quaternion Quaternion_multiplied(Quaternion quaternion1, Quaternion quaternion2) {
	Quaternion_multiply(&quaternion1, quaternion2);
	return quaternion1;
}

#define SLERP_TO_LERP_SWITCH_THRESHOLD 0.01f

Quaternion Quaternion_slerp(Quaternion left, Quaternion right, float phase) {
	float leftWeight, rightWeight, difference;
	Quaternion result;
	
	difference = left.x * right.x + left.y * right.y + left.z * right.z + left.w * right.w;
	if (1.0f - fabs(difference) > SLERP_TO_LERP_SWITCH_THRESHOLD) {
		float theta, oneOverSinTheta;
		
		theta = acos(fabs(difference));
		oneOverSinTheta = 1.0f / sin(theta);
		leftWeight = sin(theta * (1.0f - phase)) * oneOverSinTheta;
		rightWeight = sin(theta * phase) * oneOverSinTheta;
		if (difference < 0.0f) {
			leftWeight = -leftWeight;
		}
	} else {
		leftWeight = 1.0f - phase;
		rightWeight = phase;
	}
	result.x = left.x * leftWeight + right.x * rightWeight;
	result.y = left.y * leftWeight + right.y * rightWeight;
	result.z = left.z * leftWeight + right.z * rightWeight;
	result.w = left.w * leftWeight + right.w * rightWeight;
	Quaternion_normalize(&result);
	
	return result;
}

void Quaternion_rotate(Quaternion * quaternion, Vector3 axis, float angle) {
	Quaternion rotationQuaternion;
	
	rotationQuaternion = Quaternion_fromAxisAngle(axis, angle);
	Quaternion_multiply(quaternion, rotationQuaternion);
}

Quaternion Quaternion_rotated(Quaternion quaternion, Vector3 axis, float angle) {
	Quaternion_rotate(&quaternion, axis, angle);
	return quaternion;
}

void Quaternion_invert(Quaternion * quaternion) {
	float length;
	
	length = 1.0f / (quaternion->x * quaternion->x +
	                 quaternion->y * quaternion->y +
	                 quaternion->z * quaternion->z +
	                 quaternion->w * quaternion->w);
	quaternion->x *= -length;
	quaternion->y *= -length;
	quaternion->z *= -length;
	quaternion->w *= length;
}

Quaternion Quaternion_inverted(Quaternion quaternion) {
	Quaternion_invert(&quaternion);
	return quaternion;
}

Vector3 Quaternion_multiplyVector3(Quaternion quaternion, Vector3 vector) {
	Quaternion vectorQuaternion, inverseQuaternion, result;
	
	vectorQuaternion = Quaternion_fromVector(vector);
	inverseQuaternion = Quaternion_inverted(quaternion);
	result = Quaternion_multiplied(quaternion, Quaternion_multiplied(vectorQuaternion, inverseQuaternion));
	return Quaternion_toVector(result);
}

Vector4 Quaternion_multiplyVector4(Quaternion quaternion, Vector4 vector) {
	Vector3 vector3;
	
	vector3 = Quaternion_multiplyVector3(quaternion, Vector3_init(vector.x, vector.y, vector.z));
	return Vector4_init(vector3.x, vector3.y, vector3.z, vector.w);
}
