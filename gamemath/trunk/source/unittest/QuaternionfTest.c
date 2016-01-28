#include <math.h>
#include <stdbool.h>
#include "gamemath/Quaternionf.h"
#include "unittest/TestSuite.h"

#define TEST_EPSILON 0.00001f

static void testLoadIdentity() {
	Quaternionf quaternion;
	
	memset(&quaternion, 0xFF, sizeof(quaternion));
	Quaternionf_loadIdentity(&quaternion);
	TestCase_assert(quaternion.x == 0.0f &&
	                quaternion.y == 0.0f &&
	                quaternion.z == 0.0f &&
	                quaternion.w == 1.0f,
	                "Incorrect quaternion values (expected {0, 0, 0, 1}; got {%f, %f, %f, %f})",
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
}

static void testIdentity() {
	Quaternionf quaternion;
	
	quaternion = QUATERNIONf_IDENTITY;
	TestCase_assert(quaternion.x == 0.0f &&
	                quaternion.y == 0.0f &&
	                quaternion.z == 0.0f &&
	                quaternion.w == 1.0f,
	                "Incorrect quaternion values (expected {0, 0, 0, 0}; got {%f, %f, %f, %f})",
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
}

static void testInit() {
	Quaternionf quaternion;
	
	quaternion = QUATERNIONf(1.2f, 3.4f, 5.6f, 7.8f);
	TestCase_assert(fabs(quaternion.x - 1.2f) < TEST_EPSILON &&
	                fabs(quaternion.y - 3.4f) < TEST_EPSILON &&
	                fabs(quaternion.z - 5.6f) < TEST_EPSILON &&
	                fabs(quaternion.w - 7.8f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {1.2, 3.4, 5.6, 7.8}; got {%f, %f, %f, %f})",
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	
	quaternion = QUATERNIONf(-0.1f, -0.2f, -0.3f, -0.4f);
	TestCase_assert(fabs(quaternion.x - -0.1f) < TEST_EPSILON &&
	                fabs(quaternion.y - -0.2f) < TEST_EPSILON &&
	                fabs(quaternion.z - -0.3f) < TEST_EPSILON &&
	                fabs(quaternion.w - -0.4f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {-0.1, -0.2, -0.3, -0.4}; got {%f, %f, %f, %f})",
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
}

static void testFromAxisAngle() {
	Quaternionf quaternion;
	
	quaternion = Quaternionf_fromAxisAngle(VECTOR3f_RIGHT, 0.0f);
	TestCase_assert(fabs(quaternion.x - 0.0f) < TEST_EPSILON &&
	                fabs(quaternion.y - 0.0f) < TEST_EPSILON &&
	                fabs(quaternion.z - 0.0f) < TEST_EPSILON &&
	                fabs(quaternion.w - 1.0f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                0.0f, 0.0f, 0.0f, 1.0f,
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	
	quaternion = Quaternionf_fromAxisAngle(VECTOR3f(60.0f, 60.0f, 60.0f), 1.0f);
	TestCase_assert(fabs(quaternion.x - 0.276796f) < TEST_EPSILON &&
	                fabs(quaternion.y - 0.276796f) < TEST_EPSILON &&
	                fabs(quaternion.z - 0.276796f) < TEST_EPSILON &&
	                fabs(quaternion.w - 0.877583f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                0.276796f, 0.276796f, 0.276796f, 0.877583f,
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	
	quaternion = Quaternionf_fromAxisAngle(VECTOR3f(0.0f, -0.1f, 0.0f), M_PI);
	TestCase_assert(fabs(quaternion.x - 0.0f) < TEST_EPSILON &&
	                fabs(quaternion.y - -1.0f) < TEST_EPSILON &&
	                fabs(quaternion.z - 0.0f) < TEST_EPSILON &&
	                fabs(quaternion.w - 0.0f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                0.0f, -1.0f, 0.0f, 0.0f,
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	
	quaternion = Quaternionf_fromAxisAngle(VECTOR3f(0.0f, -0.1f, 0.0f), M_PI / 2);
	TestCase_assert(fabs(quaternion.x - 0.0f) < TEST_EPSILON &&
	                fabs(quaternion.y - -0.707107f) < TEST_EPSILON &&
	                fabs(quaternion.z - 0.0f) < TEST_EPSILON &&
	                fabs(quaternion.w - 0.707107f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                0.0f, -0.707107f, 0.0f, 0.707107f,
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	
	quaternion = Quaternionf_fromAxisAngle(VECTOR3f_ZERO, 0.0f);
	TestCase_assert(isnan(quaternion.x) &&
	                isnan(quaternion.y) &&
	                isnan(quaternion.z) &&
	                fabs(quaternion.w - 1.0f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                NAN, NAN, NAN, 1.0f,
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
}

static void testToAxisAngle() {
	Vector3f axis;
	float angle;
	
	Quaternionf_toAxisAngle(QUATERNIONf(0.0f, 0.0f, 0.0f, 1.0f), &axis, &angle);
	TestCase_assert(fabs(axis.x - 0.0f) < TEST_EPSILON &&
	                fabs(axis.y - 0.0f) < TEST_EPSILON &&
	                fabs(axis.z - 0.0f) < TEST_EPSILON &&
	                fabs(angle - 0.0f) < TEST_EPSILON,
	                "Incorrect axis/angle values (expected {%f, %f, %f}, %f; got {%f, %f, %f}, %f)",
	                0.0f, 0.0f, 0.0f, 0.0f,
	                axis.x, axis.y, axis.z, angle);
	
	Quaternionf_toAxisAngle(QUATERNIONf(0.276796f, 0.276796f, 0.276796f, 0.877583f), &axis, &angle);
	TestCase_assert(fabs(axis.x - 0.577350269189626f) < TEST_EPSILON &&
	                fabs(axis.y - 0.577350269189626f) < TEST_EPSILON &&
	                fabs(axis.z - 0.577350269189626f) < TEST_EPSILON &&
	                fabs(angle - 1.0f) < TEST_EPSILON,
	                "Incorrect axis/angle values (expected {%f, %f, %f}, %f; got {%f, %f, %f}, %f)",
	                0.577350269189626f, 0.577350269189626f, 0.577350269189626f, 1.0f,
	                axis.x, axis.y, axis.z, angle);
	
	Quaternionf_toAxisAngle(QUATERNIONf(0.0f, -1.0f, 0.0f, 0.0f), &axis, &angle);
	TestCase_assert(fabs(axis.x - 0.0f) < TEST_EPSILON &&
	                fabs(axis.y - -1.0f) < TEST_EPSILON &&
	                fabs(axis.z - 0.0f) < TEST_EPSILON &&
	                fabs(angle - M_PI) < TEST_EPSILON,
	                "Incorrect axis/angle values (expected {%f, %f, %f}, %f; got {%f, %f, %f}, %f)",
	                0.0f, -1.0f, 0.0f, M_PI,
	                axis.x, axis.y, axis.z, angle);
	
	Quaternionf_toAxisAngle(QUATERNIONf(0.0f, -0.707107f, 0.0f, 0.707107f), &axis, &angle);
	TestCase_assert(fabs(axis.x - 0.0f) < TEST_EPSILON &&
	                fabs(axis.y - -1.0f) < TEST_EPSILON &&
	                fabs(axis.z - 0.0f) < TEST_EPSILON &&
	                fabs(angle - M_PI / 2) < TEST_EPSILON,
	                "Incorrect axis/angle values (expected {%f, %f, %f}, %f; got {%f, %f, %f}, %f)",
	                0.0f, -1.0f, 0.0f, M_PI / 2,
	                axis.x, axis.y, axis.z, angle);
	
	Quaternionf_toAxisAngle(QUATERNIONf(0.0f, 0.0f, 0.0f, 0.0f), &axis, &angle);
	TestCase_assert(isnan(axis.x) &&
	                isnan(axis.y) &&
	                isnan(axis.z) &&
	                isnan(angle),
	                "Incorrect axis/angle values (expected {%f, %f, %f}, %f; got {%f, %f, %f}, %f)",
	                NAN, NAN, NAN, NAN,
	                axis.x, axis.y, axis.z, angle);
}

static bool QuaternionfTests_matrixCompare(Matrix4x4f matrix, float m0, float m1, float m2, float m3, float m4, float m5, float m6, float m7, float m8, float m9, float m10, float m11, float m12, float m13, float m14, float m15) {
	return fabs(matrix.m[0] - m0) < TEST_EPSILON &&
	       fabs(matrix.m[1] - m1) < TEST_EPSILON &&
	       fabs(matrix.m[2] - m2) < TEST_EPSILON &&
	       fabs(matrix.m[3] - m3) < TEST_EPSILON &&
	       fabs(matrix.m[4] - m4) < TEST_EPSILON &&
	       fabs(matrix.m[5] - m5) < TEST_EPSILON &&
	       fabs(matrix.m[6] - m6) < TEST_EPSILON &&
	       fabs(matrix.m[7] - m7) < TEST_EPSILON &&
	       fabs(matrix.m[8] - m8) < TEST_EPSILON &&
	       fabs(matrix.m[9] - m9) < TEST_EPSILON &&
	       fabs(matrix.m[10] - m10) < TEST_EPSILON &&
	       fabs(matrix.m[11] - m11) < TEST_EPSILON &&
	       fabs(matrix.m[12] - m12) < TEST_EPSILON &&
	       fabs(matrix.m[13] - m13) < TEST_EPSILON &&
	       fabs(matrix.m[14] - m14) < TEST_EPSILON &&
	       fabs(matrix.m[15] - m15) < TEST_EPSILON;
}

static void testToMatrix() {
	Matrix4x4f matrix;
	
	matrix = Quaternionf_toMatrix(QUATERNIONf(0.0f, 0.0f, 0.0f, 1.0f));
	TestCase_assert(QuaternionfTests_matrixCompare(matrix,
	                                              1.0f, 0.0f, 0.0f, 0.0f,
	                                              0.0f, 1.0f, 0.0f, 0.0f,
	                                              0.0f, 0.0f, 1.0f, 0.0f,
	                                              0.0f, 0.0f, 0.0f, 1.0f),
	                "Incorrect matrix values (expected {{%f, %f, %f, %f}, {%f, %f, %f, %f}, {%f, %f, %f, %f}, {%f, %f, %f, %f}}; got {{%f, %f, %f, %f}, {%f, %f, %f, %f}, {%f, %f, %f, %f}, {%f, %f, %f, %f}})",
	                1.0f, 0.0f, 0.0f, 0.0f,
	                0.0f, 1.0f, 0.0f, 0.0f,
	                0.0f, 0.0f, 1.0f, 0.0f,
	                0.0f, 0.0f, 0.0f, 1.0f,
	                matrix.m[0], matrix.m[1], matrix.m[2], matrix.m[3],
	                matrix.m[4], matrix.m[5], matrix.m[6], matrix.m[7],
	                matrix.m[8], matrix.m[9], matrix.m[10], matrix.m[11],
	                matrix.m[12], matrix.m[13], matrix.m[14], matrix.m[15]);
	
	matrix = Quaternionf_toMatrix(QUATERNIONf(0.0f, 0.707107f, 0.0f, 0.707107f));
	TestCase_assert(QuaternionfTests_matrixCompare(matrix,
	                                              0.0f, 0.0f, -1.0f, 0.0f,
	                                              0.0f, 1.0f, 0.0f, 0.0f,
	                                              1.0f, 0.0f, 0.0f, 0.0f,
	                                              0.0f, 0.0f, 0.0f, 1.0f),
	                "Incorrect matrix values (expected {{%f, %f, %f, %f}, {%f, %f, %f, %f}, {%f, %f, %f, %f}, {%f, %f, %f, %f}}; got {{%f, %f, %f, %f}, {%f, %f, %f, %f}, {%f, %f, %f, %f}, {%f, %f, %f, %f}})",
	                0.0f, 0.0f, -1.0f, 0.0f,
	                0.0f, 1.0f, 0.0f, 0.0f,
	                1.0f, 0.0f, 0.0f, 0.0f,
	                0.0f, 0.0f, 0.0f, 1.0f,
	                matrix.m[0], matrix.m[1], matrix.m[2], matrix.m[3],
	                matrix.m[4], matrix.m[5], matrix.m[6], matrix.m[7],
	                matrix.m[8], matrix.m[9], matrix.m[10], matrix.m[11],
	                matrix.m[12], matrix.m[13], matrix.m[14], matrix.m[15]);
	
	matrix = Quaternionf_toMatrix(QUATERNIONf(0.5f, 0.5f, 0.5f, 0.5f));
	TestCase_assert(QuaternionfTests_matrixCompare(matrix,
	                                              0.0f, 1.0f, 0.0f, 0.0f,
	                                              0.0f, 0.0f, 1.0f, 0.0f,
	                                              1.0f, 0.0f, 0.0f, 0.0f,
	                                              0.0f, 0.0f, 0.0f, 1.0f),
	                "Incorrect matrix values (expected {{%f, %f, %f, %f}, {%f, %f, %f, %f}, {%f, %f, %f, %f}, {%f, %f, %f, %f}}; got {{%f, %f, %f, %f}, {%f, %f, %f, %f}, {%f, %f, %f, %f}, {%f, %f, %f, %f}})",
	                0.0f, 1.0f, 0.0f, 0.0f,
	                0.0f, 0.0f, 1.0f, 0.0f,
	                1.0f, 0.0f, 0.0f, 0.0f,
	                0.0f, 0.0f, 0.0f, 1.0f,
	                matrix.m[0], matrix.m[1], matrix.m[2], matrix.m[3],
	                matrix.m[4], matrix.m[5], matrix.m[6], matrix.m[7],
	                matrix.m[8], matrix.m[9], matrix.m[10], matrix.m[11],
	                matrix.m[12], matrix.m[13], matrix.m[14], matrix.m[15]);
	
	matrix = Quaternionf_toMatrix(QUATERNIONf(1.0f, 1.0f, 1.0f, 1.0f));
	TestCase_assert(QuaternionfTests_matrixCompare(matrix,
	                                              -3.0f, 4.0f, 0.0f, 0.0f,
	                                              0.0f, -3.0f, 4.0f, 0.0f,
	                                              4.0f, 0.0f, -3.0f, 0.0f,
	                                              0.0f, 0.0f, 0.0f, 1.0f),
	                "Incorrect matrix values (expected {{%f, %f, %f, %f}, {%f, %f, %f, %f}, {%f, %f, %f, %f}, {%f, %f, %f, %f}}; got {{%f, %f, %f, %f}, {%f, %f, %f, %f}, {%f, %f, %f, %f}, {%f, %f, %f, %f}})",
	                -3.0f, 4.0f, 0.0f, 0.0f,
	                0.0f, -3.0f, 4.0f, 0.0f,
	                4.0f, 0.0f, -3.0f, 0.0f,
	                0.0f, 0.0f, 0.0f, 1.0f,
	                matrix.m[0], matrix.m[1], matrix.m[2], matrix.m[3],
	                matrix.m[4], matrix.m[5], matrix.m[6], matrix.m[7],
	                matrix.m[8], matrix.m[9], matrix.m[10], matrix.m[11],
	                matrix.m[12], matrix.m[13], matrix.m[14], matrix.m[15]);
}

static void testNormalize() {
	Quaternionf quaternion;
	
	quaternion = QUATERNIONf(1.0f, -1.0f, 1.0f, -1.0f);
	Quaternionf_normalize(&quaternion);
	TestCase_assert(fabs(quaternion.x - 0.5f) < TEST_EPSILON &&
	                fabs(quaternion.y - -0.5f) < TEST_EPSILON &&
	                fabs(quaternion.z - 0.5f) < TEST_EPSILON &&
	                fabs(quaternion.w - -0.5f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                0.5f, -0.5f, 0.5f, -0.5f,
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	Quaternionf_normalize(&quaternion);
	TestCase_assert(fabs(quaternion.x - 0.5f) < TEST_EPSILON &&
	                fabs(quaternion.y - -0.5f) < TEST_EPSILON &&
	                fabs(quaternion.z - 0.5f) < TEST_EPSILON &&
	                fabs(quaternion.w - -0.5f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                0.5f, -0.5f, 0.5f, -0.5f,
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
}

static void testNormalized() {
	Quaternionf quaternion;
	
	quaternion = Quaternionf_normalized(QUATERNIONf(1.0f, -1.0f, 1.0f, -1.0f));
	TestCase_assert(fabs(quaternion.x - 0.5f) < TEST_EPSILON &&
	                fabs(quaternion.y - -0.5f) < TEST_EPSILON &&
	                fabs(quaternion.z - 0.5f) < TEST_EPSILON &&
	                fabs(quaternion.w - -0.5f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                0.5f, -0.5f, 0.5f, -0.5f,
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	quaternion = Quaternionf_normalized(QUATERNIONf(0.5f, -0.5f, 0.5f, -0.5f));
	TestCase_assert(fabs(quaternion.x - 0.5f) < TEST_EPSILON &&
	                fabs(quaternion.y - -0.5f) < TEST_EPSILON &&
	                fabs(quaternion.z - 0.5f) < TEST_EPSILON &&
	                fabs(quaternion.w - -0.5f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                0.5f, -0.5f, 0.5f, -0.5f,
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
}

static void testMultiply() {
	Quaternionf quaternion;
	
	quaternion = QUATERNIONf(0.0f, 0.0f, 0.0f, 1.0f);
	Quaternionf_multiply(&quaternion, QUATERNIONf(0.0f, 0.0f, 0.0f, 1.0f));
	TestCase_assert(fabs(quaternion.x - 0.0f) < TEST_EPSILON &&
	                fabs(quaternion.y - 0.0f) < TEST_EPSILON &&
	                fabs(quaternion.z - 0.0f) < TEST_EPSILON &&
	                fabs(quaternion.w - 1.0f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                0.0f, 0.0f, 0.0f, 1.0f,
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	
	quaternion = QUATERNIONf(-1.0f, -1.0f, 0.0f, 1.0f);
	Quaternionf_multiply(&quaternion, QUATERNIONf(-0.5f, 0.5f, 0.5f, 0.5f));
	TestCase_assert(fabs(quaternion.x - -1.5f) < TEST_EPSILON &&
	                fabs(quaternion.y - 0.5f) < TEST_EPSILON &&
	                fabs(quaternion.z - -0.5f) < TEST_EPSILON &&
	                fabs(quaternion.w - 0.5f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                -1.5f, 0.5f, -0.5f, 0.5f,
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
}

static void testMultiplied() {
	Quaternionf quaternion;
	
	quaternion = Quaternionf_multiplied(QUATERNIONf(0.0f, 0.0f, 0.0f, 1.0f),
	                                    QUATERNIONf(0.0f, 0.0f, 0.0f, 1.0f));
	TestCase_assert(fabs(quaternion.x - 0.0f) < TEST_EPSILON &&
	                fabs(quaternion.y - 0.0f) < TEST_EPSILON &&
	                fabs(quaternion.z - 0.0f) < TEST_EPSILON &&
	                fabs(quaternion.w - 1.0f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                0.0f, 0.0f, 0.0f, 1.0f,
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	
	quaternion = Quaternionf_multiplied(QUATERNIONf(-1.0f, -1.0f, 0.0f, 1.0f),
	                                    QUATERNIONf(-0.5f, 0.5f, 0.5f, 0.5f));
	TestCase_assert(fabs(quaternion.x - -1.5f) < TEST_EPSILON &&
	                fabs(quaternion.y - 0.5f) < TEST_EPSILON &&
	                fabs(quaternion.z - -0.5f) < TEST_EPSILON &&
	                fabs(quaternion.w - 0.5f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                -1.5f, 0.5f, -0.5f, 0.5f,
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
}

static void testSlerp() {
	Quaternionf quaternion;
	
	quaternion = Quaternionf_slerp(QUATERNIONf(1.0f, 0.0f, 0.0f, 0.0f),
	                               QUATERNIONf(0.0f, 1.0f, 0.0f, 0.0f),
	                               0.0f);
	TestCase_assert(fabs(quaternion.x - 1.0f) < TEST_EPSILON &&
	                fabs(quaternion.y - 0.0f) < TEST_EPSILON &&
	                fabs(quaternion.z - 0.0f) < TEST_EPSILON &&
	                fabs(quaternion.w - 0.0f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                1.0f, 0.0f, 0.0f, 0.0f,
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	
	quaternion = Quaternionf_slerp(QUATERNIONf(1.0f, 0.0f, 0.0f, 0.0f),
	                               QUATERNIONf(0.0f, 1.0f, 0.0f, 0.0f),
	                               1.0f);
	TestCase_assert(fabs(quaternion.x - 0.0f) < TEST_EPSILON &&
	                fabs(quaternion.y - 1.0f) < TEST_EPSILON &&
	                fabs(quaternion.z - 0.0f) < TEST_EPSILON &&
	                fabs(quaternion.w - 0.0f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                0.0f, 1.0f, 0.0f, 0.0f,
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	
	quaternion = Quaternionf_slerp(QUATERNIONf(0.707107f, 0.0f, 0.0f, 0.707107f),
	                               QUATERNIONf(0.0f, 0.707107f, 0.0f, 0.707107f),
	                               0.5f);
	TestCase_assert(fabs(quaternion.x - 0.408248290463863f) < TEST_EPSILON &&
	                fabs(quaternion.y - 0.408248290463863f) < TEST_EPSILON &&
	                fabs(quaternion.z - 0.0f) < TEST_EPSILON &&
	                fabs(quaternion.w - 0.816496580927725f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                0.408248290463863f, 0.408248290463863f, 0.0f, 0.816496580927725f,
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	
	quaternion = Quaternionf_slerp(QUATERNIONf(0.707107f, 0.0f, 0.0f, 0.707107f),
	                               QUATERNIONf(0.0f, 0.707107f, 0.0f, 0.707107f),
	                               0.75f);
	TestCase_assert(fabs(quaternion.x - 0.211325f) < TEST_EPSILON &&
	                fabs(quaternion.y - 0.577350f) < TEST_EPSILON &&
	                fabs(quaternion.z - 0.0f) < TEST_EPSILON &&
	                fabs(quaternion.w - 0.788675f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                0.211325f, 0.577350f, 0.0f, 0.788675f,
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
}

static void testRotate() {
	Quaternionf quaternion;
	
	quaternion = QUATERNIONf_IDENTITY;
	Quaternionf_rotate(&quaternion, VECTOR3f_UP, M_PI / 5);
	TestCase_assert(fabs(quaternion.x - 0.0f) < TEST_EPSILON &&
	                fabs(quaternion.y - 0.309017f) < TEST_EPSILON &&
	                fabs(quaternion.z - 0.0f) < TEST_EPSILON &&
	                fabs(quaternion.w - 0.951057f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                0.0f, 0.309017f, 0.0f, 0.951057f,
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	
	Quaternionf_rotate(&quaternion, VECTOR3f(1.0f, 0.0f, 0.5f), 1.0f);
	TestCase_assert(fabs(quaternion.x - 0.474079f) < TEST_EPSILON &&
	                fabs(quaternion.y - 0.271188f) < TEST_EPSILON &&
	                fabs(quaternion.z - 0.071402f) < TEST_EPSILON &&
	                fabs(quaternion.w - 0.834631f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                0.474079f, 0.271188f, 0.071402f, 0.834631f,
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
}

static void testRotated() {
	Quaternionf quaternion;
	
	quaternion = QUATERNIONf_IDENTITY;
	quaternion = Quaternionf_rotated(quaternion, VECTOR3f_UP, M_PI / 5);
	TestCase_assert(fabs(quaternion.x - 0.0f) < TEST_EPSILON &&
	                fabs(quaternion.y - 0.309017f) < TEST_EPSILON &&
	                fabs(quaternion.z - 0.0f) < TEST_EPSILON &&
	                fabs(quaternion.w - 0.951057f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                0.0f, 0.309017f, 0.0f, 0.951057f,
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	
	quaternion = Quaternionf_rotated(quaternion, VECTOR3f(1.0f, 0.0f, 0.5f), 1.0f);
	TestCase_assert(fabs(quaternion.x - 0.474079f) < TEST_EPSILON &&
	                fabs(quaternion.y - 0.271188f) < TEST_EPSILON &&
	                fabs(quaternion.z - 0.071402f) < TEST_EPSILON &&
	                fabs(quaternion.w - 0.834631f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                0.474079f, 0.271188f, 0.071402f, 0.834631f,
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
}

static void testInvert() {
	Quaternionf quaternion;
	
	quaternion = QUATERNIONf(0.0f, 1.0f, 0.0f, 0.0f);
	Quaternionf_invert(&quaternion);
	TestCase_assert(fabs(quaternion.x - 0.0f) < TEST_EPSILON &&
	                fabs(quaternion.y - -1.0f) < TEST_EPSILON &&
	                fabs(quaternion.z - 0.0f) < TEST_EPSILON &&
	                fabs(quaternion.w - 0.0f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                0.0f, -1.0f, 0.0f, 0.0f,
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	
	quaternion = QUATERNIONf(-0.5f, 0.5f, 1.0f, 0.5f);
	Quaternionf_invert(&quaternion);
	TestCase_assert(fabs(quaternion.x - 0.285714f) < TEST_EPSILON &&
	                fabs(quaternion.y - -0.285714f) < TEST_EPSILON &&
	                fabs(quaternion.z - -0.571429f) < TEST_EPSILON &&
	                fabs(quaternion.w - 0.285714f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                0.285714f, -0.285714f, -0.571429f, 0.285714f,
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	
	Quaternionf_invert(&quaternion);
	TestCase_assert(fabs(quaternion.x - -0.5f) < TEST_EPSILON &&
	                fabs(quaternion.y - 0.5f) < TEST_EPSILON &&
	                fabs(quaternion.z - 1.0f) < TEST_EPSILON &&
	                fabs(quaternion.w - 0.5f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                -0.5f, 0.5f, 1.0f, 0.5f,
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
}

static void testInverted() {
	Quaternionf quaternion;
	
	quaternion = QUATERNIONf(0.0f, 1.0f, 0.0f, 0.0f);
	quaternion = Quaternionf_inverted(quaternion);
	TestCase_assert(fabs(quaternion.x - 0.0f) < TEST_EPSILON &&
	                fabs(quaternion.y - -1.0f) < TEST_EPSILON &&
	                fabs(quaternion.z - 0.0f) < TEST_EPSILON &&
	                fabs(quaternion.w - 0.0f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                0.0f, -1.0f, 0.0f, 0.0f,
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	
	quaternion = QUATERNIONf(-0.5f, 0.5f, 1.0f, 0.5f);
	quaternion = Quaternionf_inverted(quaternion);
	TestCase_assert(fabs(quaternion.x - 0.285714f) < TEST_EPSILON &&
	                fabs(quaternion.y - -0.285714f) < TEST_EPSILON &&
	                fabs(quaternion.z - -0.571429f) < TEST_EPSILON &&
	                fabs(quaternion.w - 0.285714f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                0.285714f, -0.285714f, -0.571429f, 0.285714f,
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	
	quaternion = Quaternionf_inverted(quaternion);
	TestCase_assert(fabs(quaternion.x - -0.5f) < TEST_EPSILON &&
	                fabs(quaternion.y - 0.5f) < TEST_EPSILON &&
	                fabs(quaternion.z - 1.0f) < TEST_EPSILON &&
	                fabs(quaternion.w - 0.5f) < TEST_EPSILON,
	                "Incorrect quaternion values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                -0.5f, 0.5f, 1.0f, 0.5f,
	                quaternion.x, quaternion.y, quaternion.z, quaternion.w);
}

static void testMultiplyVector3f() {
	Vector3f vector;
	
	vector = Quaternionf_multiplyVector3f(Quaternionf_fromAxisAngle(VECTOR3f_UP, M_PI / 4),
	                                      VECTOR3f(1.0f, 1.0f, 0.0f));
	TestCase_assert(fabs(vector.x - 0.707107f) < TEST_EPSILON &&
	                fabs(vector.y - 1.0f) < TEST_EPSILON &&
	                fabs(vector.z - -0.707107f) < TEST_EPSILON,
	                "Incorrect vector values (expected {%f, %f, %f}; got {%f, %f, %f})",
	                0.707107f, 1.0f, -0.707107f,
	                vector.x, vector.y, vector.z);
	
	vector = Quaternionf_multiplyVector3f(Quaternionf_fromAxisAngle(VECTOR3f_RIGHT, M_PI / 4),
	                                      VECTOR3f(-2.5f, 2.0f, 0.0f));
	TestCase_assert(fabs(vector.x - -2.5f) < TEST_EPSILON &&
	                fabs(vector.y - 1.414213f) < TEST_EPSILON &&
	                fabs(vector.z - 1.414213f) < TEST_EPSILON,
	                "Incorrect vector values (expected {%f, %f, %f}; got {%f, %f, %f})",
	                -2.5f, 1.414213f, 1.414213f,
	                vector.x, vector.y, vector.z);
	
	vector = Quaternionf_multiplyVector3f(Quaternionf_fromAxisAngle(VECTOR3f_FRONT, M_PI / 4),
	                                      VECTOR3f(1.0f, 1.0f, 0.0f));
	TestCase_assert(fabs(vector.x - 0.0f) < TEST_EPSILON &&
	                fabs(vector.y - 1.414213f) < TEST_EPSILON &&
	                fabs(vector.z - 0.0f) < TEST_EPSILON,
	                "Incorrect vector values (expected {%f, %f, %f}; got {%f, %f, %f})",
	                0.0f, 1.414213f, 0.0f,
	                vector.x, vector.y, vector.z);
}

static void testMultiplyVector4f() {
	Vector4f vector;
	
	vector = Quaternionf_multiplyVector4f(Quaternionf_fromAxisAngle(VECTOR3f_UP, M_PI / 4),
	                                      VECTOR4f(1.0f, 1.0f, 0.0f, 1.0f));
	TestCase_assert(fabs(vector.x - 0.707107f) < TEST_EPSILON &&
	                fabs(vector.y - 1.0f) < TEST_EPSILON &&
	                fabs(vector.z - -0.707107f) < TEST_EPSILON &&
	                fabs(vector.w - 1.0f) < TEST_EPSILON,
	                "Incorrect vector values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                0.707107f, 1.0f, -0.707107f, 1.0f,
	                vector.x, vector.y, vector.z, vector.w);
	
	vector = Quaternionf_multiplyVector4f(Quaternionf_fromAxisAngle(VECTOR3f_RIGHT, M_PI / 4),
	                                      VECTOR4f(-2.5f, 2.0f, 0.0f, 0.0f));
	TestCase_assert(fabs(vector.x - -2.5f) < TEST_EPSILON &&
	                fabs(vector.y - 1.414213f) < TEST_EPSILON &&
	                fabs(vector.z - 1.414213f) < TEST_EPSILON &&
	                fabs(vector.w - 0.0f) < TEST_EPSILON,
	                "Incorrect vector values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                -2.5f, 1.414213f, 1.414213f, 0.0f,
	                vector.x, vector.y, vector.z, vector.w);
	
	vector = Quaternionf_multiplyVector4f(Quaternionf_fromAxisAngle(VECTOR3f_FRONT, M_PI / 4),
	                                      VECTOR4f(1.0f, 1.0f, 0.0f, 0.0f));
	TestCase_assert(fabs(vector.x - 0.0f) < TEST_EPSILON &&
	                fabs(vector.y - 1.414213f) < TEST_EPSILON &&
	                fabs(vector.z - 0.0f) < TEST_EPSILON &&
	                fabs(vector.w - 0.0f) < TEST_EPSILON,
	                "Incorrect vector values (expected {%f, %f, %f, %f}; got {%f, %f, %f, %f})",
	                0.0f, 1.414213f, 0.0f, 0.0f,
	                vector.x, vector.y, vector.z, vector.w);
}

TEST_SUITE(QuaternionfTest,
           testLoadIdentity,
           testIdentity,
           testInit,
           testFromAxisAngle,
           testToAxisAngle,
           testToMatrix,
           testNormalize,
           testNormalized,
           testMultiply,
           testMultiplied,
           testSlerp,
           testRotate,
           testRotated,
           testInvert,
           testInverted,
           testMultiplyVector3f,
           testMultiplyVector4f)