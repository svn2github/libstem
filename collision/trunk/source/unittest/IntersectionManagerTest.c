#include "collision/IntersectionManager.h"
#include "collision/StandardIntersectionHandlers.h"
#include "unittest/TestSuite.h"

static void verifyInit(int line, IntersectionManager * intersectionManager) {
	TestCase_assert(intersectionManager != NULL, "Expected non-NULL but got NULL (line %d)", line);
	TestCase_assert(intersectionManager->dispose == IntersectionManager_dispose, "Expected %p but got %p (line %d)", IntersectionManager_dispose, intersectionManager->dispose, line);
	TestCase_assert(intersectionManager->defaultHandler == NULL, "Expected NULL but got %p (line %d)", intersectionManager->defaultHandler, line);
}

static void testInit() {
	IntersectionManager intersectionManager, * intersectionManagerPtr;
	
	memset(&intersectionManager, 0x00, sizeof(intersectionManager));
	IntersectionManager_init(&intersectionManager);
	verifyInit(__LINE__, &intersectionManager);
	IntersectionManager_dispose(&intersectionManager);
	
	memset(&intersectionManager, 0xFF, sizeof(intersectionManager));
	IntersectionManager_init(&intersectionManager);
	verifyInit(__LINE__, &intersectionManager);
	IntersectionManager_dispose(&intersectionManager);
	
	intersectionManagerPtr = IntersectionManager_create();
	verifyInit(__LINE__, intersectionManagerPtr);
	IntersectionManager_dispose(intersectionManagerPtr);
}

static unsigned int handler1Calls, handler2Calls;

static bool testHandler1(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal, Vector3x * outObject1Vector, Vector3x * outObject2Vector, fixed16_16 * outContactArea) {
	handler1Calls++;
	return false;
}

static bool testHandler2(CollisionObject * object1, CollisionObject * object2, fixed16_16 * outTime, Vector3x * outNormal, Vector3x * outObject1Vector, Vector3x * outObject2Vector, fixed16_16 * outContactArea) {
	handler2Calls++;
	*outTime = 1;
	*outNormal = VECTOR3x(2, 3, 4);
	*outObject1Vector = VECTOR3x(5, 6, 7);
	*outObject2Vector = VECTOR3x(8, 9, 10);
	*outContactArea = 11;
	return true;
}

static void testGetHandler() {
	IntersectionManager * intersectionManager;
	IntersectionHandler handler;
	
	intersectionManager = IntersectionManager_create();
	intersectionManager->defaultHandler = (void *) 0x1;
	handler = IntersectionManager_getHandler(intersectionManager, 0, 0);
	TestCase_assert(handler == (void *) 0x1, "Expected 0x1 but got %p", handler);
	intersectionManager->defaultHandler = NULL;
	handler = IntersectionManager_getHandler(intersectionManager, 0, 0);
	TestCase_assert(handler == NULL, "Expected NULL but got %p", handler);
	
	IntersectionManager_setHandler(intersectionManager, 0, 0, testHandler1);
	IntersectionManager_setHandler(intersectionManager, 0, 1, testHandler2);
	IntersectionManager_setHandler(intersectionManager, 1, 1, testHandler1);
	
	handler = IntersectionManager_getHandler(intersectionManager, 0, 0);
	TestCase_assert(handler == testHandler1, "Expected %p but got %p", testHandler1, handler);
	handler = IntersectionManager_getHandler(intersectionManager, 0, 1);
	TestCase_assert(handler == testHandler2, "Expected %p but got %p", testHandler2, handler);
	handler = IntersectionManager_getHandler(intersectionManager, 1, 1);
	TestCase_assert(handler == testHandler1, "Expected %p but got %p", testHandler1, handler);
	handler = IntersectionManager_getHandler(intersectionManager, 1, 0);
	TestCase_assert(handler == NULL, "Expected NULL but got %p", handler);
	
	IntersectionManager_setHandler(intersectionManager, 0, 0, NULL);
	handler = IntersectionManager_getHandler(intersectionManager, 0, 0);
	TestCase_assert(handler == NULL, "Expected NULL but got %p", handler);
	
	IntersectionManager_dispose(intersectionManager);
}

static void testCallHandler() {
	IntersectionManager * intersectionManager;
	CollisionObject * object1, * object2;
	fixed16_16 time, contactArea;
	Vector3x normal, object1Vector, object2Vector;
	bool result;
	
	intersectionManager = IntersectionManager_create();
	handler1Calls = 0;
	handler2Calls = 0;
	object1 = CollisionObject_create(NULL, 0, NULL);
	object2 = CollisionObject_create(NULL, 0, NULL);
	time = 0;
	normal = VECTOR3x_ZERO;
	object1Vector = VECTOR3x_ZERO;
	object2Vector = VECTOR3x_ZERO;
	contactArea = 0;
	result = IntersectionManager_callHandler(intersectionManager, object1, object2, &time, &normal, &object1Vector, &object2Vector, &contactArea);
	TestCase_assert(!result, "Expected false but got true");
	TestCase_assert(time == 0, "Expected 0x00000 but got 0x%05X", time);
	TestCase_assert(normal.x == 0 && normal.y == 0 && normal.z == 0, "Expected {0x00000, 0x00000, 0x00000} but got {0x%05X, 0x%05X, 0x%05X}", normal.x, normal.y, normal.z);
	TestCase_assert(object1Vector.x == 0 && object1Vector.y == 0 && object1Vector.z == 0, "Expected {0x00000, 0x00000, 0x00000} but got {0x%05X, 0x%05X, 0x%05X}", object1Vector.x, object1Vector.y, object1Vector.z);
	TestCase_assert(object2Vector.x == 0 && object2Vector.y == 0 && object2Vector.z == 0, "Expected {0x00000, 0x00000, 0x00000} but got {0x%05X, 0x%05X, 0x%05X}", object2Vector.x, object2Vector.y, object2Vector.z);
	TestCase_assert(contactArea == 0, "Expected 0x00000 but got 0x%05X", contactArea);
	
	IntersectionManager_setHandler(intersectionManager, 0, 0, testHandler1);
	IntersectionManager_setHandler(intersectionManager, 0, 1, testHandler2);
	
	result = IntersectionManager_callHandler(intersectionManager, object1, object2, &time, &normal, &object1Vector, &object2Vector, &contactArea);
	TestCase_assert(handler1Calls == 1, "Expected 1 but got %u", handler1Calls);
	TestCase_assert(!result, "Expected false but got true");
	TestCase_assert(time == 0, "Expected 0x00000 but got 0x%05X", time);
	TestCase_assert(normal.x == 0 && normal.y == 0 && normal.z == 0, "Expected {0x00000, 0x00000, 0x00000} but got {0x%05X, 0x%05X, 0x%05X}", normal.x, normal.y, normal.z);
	TestCase_assert(object1Vector.x == 0 && object1Vector.y == 0 && object1Vector.z == 0, "Expected {0x00000, 0x00000, 0x00000} but got {0x%05X, 0x%05X, 0x%05X}", object1Vector.x, object1Vector.y, object1Vector.z);
	TestCase_assert(object2Vector.x == 0 && object2Vector.y == 0 && object2Vector.z == 0, "Expected {0x00000, 0x00000, 0x00000} but got {0x%05X, 0x%05X, 0x%05X}", object2Vector.x, object2Vector.y, object2Vector.z);
	TestCase_assert(contactArea == 0, "Expected 0x00000 but got 0x%05X", contactArea);
	
	object2->shapeType = 1;
	result = IntersectionManager_callHandler(intersectionManager, object1, object2, &time, &normal, &object1Vector, &object2Vector, &contactArea);
	TestCase_assert(handler2Calls == 1, "Expected 1 but got %u", handler2Calls);
	TestCase_assert(result, "Expected true but got false");
	TestCase_assert(time == 1, "Expected 0x00001 but got 0x%05X", time);
	TestCase_assert(normal.x == 2 && normal.y == 3 && normal.z == 4, "Expected {0x00002, 0x00003, 0x00004} but got {0x%05X, 0x%05X, 0x%05X}", normal.x, normal.y, normal.z);
	TestCase_assert(object1Vector.x == 5 && object1Vector.y == 6 && object1Vector.z == 7, "Expected {0x00005, 0x00006, 0x00007} but got {0x%05X, 0x%05X, 0x%05X}", object1Vector.x, object1Vector.y, object1Vector.z);
	TestCase_assert(object2Vector.x == 8 && object2Vector.y == 9 && object2Vector.z == 10, "Expected {0x00008, 0x00009, 0x0000A} but got {0x%05X, 0x%05X, 0x%05X}", object2Vector.x, object2Vector.y, object2Vector.z);
	TestCase_assert(contactArea == 11, "Expected 0x0000B but got 0x%05X", contactArea);
	
	object2->shapeType = 2;
	result = IntersectionManager_callHandler(intersectionManager, object1, object2, &time, &normal, &object1Vector, &object2Vector, &contactArea);
	TestCase_assert(handler2Calls == 1, "Expected 1 but got %u", handler2Calls);
	TestCase_assert(!result, "Expected false but got true");
	
	intersectionManager->defaultHandler = testHandler2;
	result = IntersectionManager_callHandler(intersectionManager, object1, object2, &time, &normal, &object1Vector, &object2Vector, &contactArea);
	TestCase_assert(handler2Calls == 2, "Expected 2 but got %u", handler2Calls);
	TestCase_assert(result, "Expected true but got false");
	
	IntersectionManager_dispose(intersectionManager);
}

#define verifyHandler(type1, type2, expectedHandler) \
	handler = IntersectionManager_getHandler(intersectionManager, type1, type2); \
	TestCase_assert(handler == expectedHandler, "Expected %p but got %p (line %d)", expectedHandler, handler, line);

static void verifyStandardHandlers(int line, IntersectionManager * intersectionManager) {
	IntersectionHandler handler;
	
	verifyHandler(COLLISION_SHAPE_RECT_2D, COLLISION_SHAPE_RECT_2D, intersectionHandler_rect2D_rect2D)
	verifyHandler(COLLISION_SHAPE_RECT_2D, COLLISION_SHAPE_CIRCLE, intersectionHandler_rect2D_circle)
	verifyHandler(COLLISION_SHAPE_RECT_2D, COLLISION_SHAPE_LINE_2D, intersectionHandler_rect2D_line2D)
	verifyHandler(COLLISION_SHAPE_RECT_2D, COLLISION_SHAPE_POLYGON, intersectionHandler_rect2D_polygon)
	verifyHandler(COLLISION_SHAPE_CIRCLE, COLLISION_SHAPE_CIRCLE, intersectionHandler_circle_circle)
	verifyHandler(COLLISION_SHAPE_CIRCLE, COLLISION_SHAPE_LINE_2D, intersectionHandler_circle_line2D)
	verifyHandler(COLLISION_SHAPE_CIRCLE, COLLISION_SHAPE_POLYGON, intersectionHandler_circle_polygon)
	verifyHandler(COLLISION_SHAPE_LINE_2D, COLLISION_SHAPE_LINE_2D, intersectionHandler_line2D_line2D)
	verifyHandler(COLLISION_SHAPE_LINE_2D, COLLISION_SHAPE_POLYGON, intersectionHandler_line2D_polygon)
	verifyHandler(COLLISION_SHAPE_POLYGON, COLLISION_SHAPE_POLYGON, intersectionHandler_polygon_polygon)
	
	verifyHandler(COLLISION_SHAPE_BOX_3D, COLLISION_SHAPE_BOX_3D, intersectionHandler_box3D_box3D)
	verifyHandler(COLLISION_SHAPE_BOX_3D, COLLISION_SHAPE_SPHERE, intersectionHandler_box3D_sphere)
	verifyHandler(COLLISION_SHAPE_BOX_3D, COLLISION_SHAPE_LINE_3D, intersectionHandler_box3D_line3D)
	verifyHandler(COLLISION_SHAPE_BOX_3D, COLLISION_SHAPE_CAPSULE, intersectionHandler_box3D_capsule)
	verifyHandler(COLLISION_SHAPE_BOX_3D, COLLISION_SHAPE_STATIC_TRIMESH, intersectionHandler_box3D_trimesh)
	verifyHandler(COLLISION_SHAPE_SPHERE, COLLISION_SHAPE_SPHERE, intersectionHandler_sphere_sphere)
	verifyHandler(COLLISION_SHAPE_SPHERE, COLLISION_SHAPE_LINE_3D, intersectionHandler_sphere_line3D)
	verifyHandler(COLLISION_SHAPE_SPHERE, COLLISION_SHAPE_CAPSULE, intersectionHandler_sphere_capsule)
	verifyHandler(COLLISION_SHAPE_SPHERE, COLLISION_SHAPE_STATIC_TRIMESH, intersectionHandler_sphere_trimesh)
	verifyHandler(COLLISION_SHAPE_LINE_3D, COLLISION_SHAPE_LINE_3D, intersectionHandler_line3D_line3D)
	verifyHandler(COLLISION_SHAPE_LINE_3D, COLLISION_SHAPE_CAPSULE, intersectionHandler_line3D_capsule)
	verifyHandler(COLLISION_SHAPE_LINE_3D, COLLISION_SHAPE_STATIC_TRIMESH, intersectionHandler_line3D_trimesh)
	verifyHandler(COLLISION_SHAPE_CAPSULE, COLLISION_SHAPE_CAPSULE, intersectionHandler_capsule_capsule)
	verifyHandler(COLLISION_SHAPE_CAPSULE, COLLISION_SHAPE_STATIC_TRIMESH, intersectionHandler_capsule_trimesh)
	verifyHandler(COLLISION_SHAPE_STATIC_TRIMESH, COLLISION_SHAPE_STATIC_TRIMESH, intersectionHandler_trimesh_trimesh)
}

static void verifyNoStandardHandlers(int line, IntersectionManager * intersectionManager) {
	IntersectionHandler handler;
	
	verifyHandler(COLLISION_SHAPE_RECT_2D, COLLISION_SHAPE_RECT_2D, NULL)
	verifyHandler(COLLISION_SHAPE_RECT_2D, COLLISION_SHAPE_CIRCLE, NULL)
	verifyHandler(COLLISION_SHAPE_RECT_2D, COLLISION_SHAPE_LINE_2D, NULL)
	verifyHandler(COLLISION_SHAPE_RECT_2D, COLLISION_SHAPE_POLYGON, NULL)
	verifyHandler(COLLISION_SHAPE_CIRCLE, COLLISION_SHAPE_CIRCLE, NULL)
	verifyHandler(COLLISION_SHAPE_CIRCLE, COLLISION_SHAPE_LINE_2D, NULL)
	verifyHandler(COLLISION_SHAPE_CIRCLE, COLLISION_SHAPE_POLYGON, NULL)
	verifyHandler(COLLISION_SHAPE_LINE_2D, COLLISION_SHAPE_LINE_2D, NULL)
	verifyHandler(COLLISION_SHAPE_LINE_2D, COLLISION_SHAPE_POLYGON, NULL)
	verifyHandler(COLLISION_SHAPE_POLYGON, COLLISION_SHAPE_POLYGON, NULL)
	
	verifyHandler(COLLISION_SHAPE_BOX_3D, COLLISION_SHAPE_BOX_3D, NULL)
	verifyHandler(COLLISION_SHAPE_BOX_3D, COLLISION_SHAPE_SPHERE, NULL)
	verifyHandler(COLLISION_SHAPE_BOX_3D, COLLISION_SHAPE_LINE_3D, NULL)
	verifyHandler(COLLISION_SHAPE_BOX_3D, COLLISION_SHAPE_CAPSULE, NULL)
	verifyHandler(COLLISION_SHAPE_BOX_3D, COLLISION_SHAPE_STATIC_TRIMESH, NULL)
	verifyHandler(COLLISION_SHAPE_SPHERE, COLLISION_SHAPE_SPHERE, NULL)
	verifyHandler(COLLISION_SHAPE_SPHERE, COLLISION_SHAPE_LINE_3D, NULL)
	verifyHandler(COLLISION_SHAPE_SPHERE, COLLISION_SHAPE_CAPSULE, NULL)
	verifyHandler(COLLISION_SHAPE_SPHERE, COLLISION_SHAPE_STATIC_TRIMESH, NULL)
	verifyHandler(COLLISION_SHAPE_LINE_3D, COLLISION_SHAPE_LINE_3D, NULL)
	verifyHandler(COLLISION_SHAPE_LINE_3D, COLLISION_SHAPE_CAPSULE, NULL)
	verifyHandler(COLLISION_SHAPE_LINE_3D, COLLISION_SHAPE_STATIC_TRIMESH, NULL)
	verifyHandler(COLLISION_SHAPE_CAPSULE, COLLISION_SHAPE_CAPSULE, NULL)
	verifyHandler(COLLISION_SHAPE_CAPSULE, COLLISION_SHAPE_STATIC_TRIMESH, NULL)
	verifyHandler(COLLISION_SHAPE_STATIC_TRIMESH, COLLISION_SHAPE_STATIC_TRIMESH, NULL)
}
#undef verifyHandler

static void testInitWithStandardHandlers() {
	IntersectionManager intersectionManager, * intersectionManagerPtr;
	
	intersectionManagerPtr = IntersectionManager_create();
	verifyInit(__LINE__, intersectionManagerPtr);
	verifyNoStandardHandlers(__LINE__, intersectionManagerPtr);
	IntersectionManager_addStandardHandlers(intersectionManagerPtr);
	verifyStandardHandlers(__LINE__, intersectionManagerPtr);
	IntersectionManager_dispose(intersectionManagerPtr);
	
	IntersectionManager_init(&intersectionManager);
	verifyInit(__LINE__, &intersectionManager);
	verifyNoStandardHandlers(__LINE__, &intersectionManager);
	IntersectionManager_dispose(&intersectionManager);
	
	IntersectionManager_initWithStandardHandlers(&intersectionManager);
	verifyInit(__LINE__, &intersectionManager);
	verifyStandardHandlers(__LINE__, &intersectionManager);
	IntersectionManager_dispose(&intersectionManager);
	
	intersectionManagerPtr = IntersectionManager_create();
	verifyInit(__LINE__, intersectionManagerPtr);
	verifyNoStandardHandlers(__LINE__, intersectionManagerPtr);
	IntersectionManager_dispose(intersectionManagerPtr);
	
	intersectionManagerPtr = IntersectionManager_createWithStandardHandlers();
	verifyInit(__LINE__, intersectionManagerPtr);
	verifyStandardHandlers(__LINE__, intersectionManagerPtr);
	IntersectionManager_dispose(intersectionManagerPtr);
}

TEST_SUITE(IntersectionManagerTest,
           testInit,
           testGetHandler,
           testCallHandler,
           testInitWithStandardHandlers)
