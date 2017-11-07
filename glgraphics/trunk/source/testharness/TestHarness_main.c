#include "gamemath/Matrix4x4f.h"
#include "glgraphics/Animation.h"
#include "glgraphics/GLIncludes.h"
#include "glgraphics/MeshRenderable.h"
#include "glgraphics/OrbitCameraController.h"
#include "glgraphics/Renderer.h"
#include "shell/Shell.h"
#include "shell/ShellCallbacks.h"
#include "shell/ShellKeyCodes.h"

#if defined(STEM_PLATFORM_macosx)
#include "nsopenglshell/NSOpenGLShell.h"
#include "nsopenglshell/NSOpenGLTarget.h"
#elif defined(STEM_PLATFORM_iphonesimulator) || defined(STEM_PLATFORM_iphoneos)
#include "eaglshell/EAGLShell.h"
#include "eaglshell/EAGLTarget.h"
#elif defined(STEM_PLATFORM_win32) || defined(STEM_PLATFORM_win64)
#include "wglshell/WGLShell.h"
#include "wglshell/WGLTarget.h"
#elif defined(STEM_PLATFORM_linux32) || defined(STEM_PLATFORM_linux64)
#include "glxshell/GLXShell.h"
#include "glxshell/GLXTarget.h"
#else
#include "glutshell/GLUTTarget.h"
#endif

#include <stdio.h>
#include <unistd.h>

static Renderer * renderer;
static MeshRenderable * renderable;
static Animation * animation;
static OrbitCameraController * cameraController;
static bool shiftKeyDown, controlKeyDown;
static unsigned int viewWidth = 1280, viewHeight = 720;
static double animationStartTime;

static bool Target_draw() {
	if (renderable->animationState != NULL) {
		Animation_setAnimationStateAtTime(animation, renderable->animationState, Shell_getCurrentTime() - animationStartTime);
		Shell_redisplay();
	}
	
	Renderer_clear(renderer);
	Renderer_setViewMatrix(renderer, OrbitCameraController_getMatrix(cameraController));
	Renderer_beginDrawing(renderer);
	Renderer_drawLayer(renderer, RENDER_LAYER_3D_OPAQUE);
	Renderer_endDrawing(renderer);
	return true;
}

static void initScene1() {
	struct vertex_p3f_t2f_n3f_c4f vertices[] = {
		{{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		{{1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		{{-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}
	};
	GLuint indexes[] = {
		0, 1, 2
	};
	
	if (renderer != NULL) {
		Renderer_dispose(renderer);
	}
	renderer = Renderer_create();
	Renderer_setClearColor(renderer, COLOR4f(0.0f, 0.125f, 0.25f, 0.0f));
	Renderer_setLights(renderer, VECTOR3f(0.0f, 8.0f, 8.0f), COLOR4f(1.0f, 1.0f, 0.95f, 1.0f), VECTOR3f(-1.0f, -2.0f, -8.0f), COLOR4f(0.8f, 0.8f, 0.8f, 1.0f), COLOR4f(0.1f, 0.1f, 0.105f, 1.0f));
	Renderer_setProjectionMatrix(renderer, Matrix4x4f_perspective(MATRIX4x4f_IDENTITY, 60.0f, (float) viewWidth / (float) viewHeight, 0.5f, 100.0f));
	
	if (renderable != NULL) {
		MeshRenderable_dispose(renderable);
	}
	renderable = MeshRenderable_createStatic(vertices, sizeof(vertices) / sizeof(struct vertex_p3f_t2f_n3f_c4f), indexes, sizeof(indexes) / sizeof(GLuint), NULL);
	Renderer_addRenderable(renderer, RENDER_LAYER_3D_OPAQUE, (Renderable *) renderable);
	
	if (cameraController != NULL) {
		OrbitCameraController_dispose(cameraController);
	}
	cameraController = OrbitCameraController_create();
}

static void initScene2() {
	struct vertex_p3f_t2f_n3f_c4f_b4u_w4f vertices[] = {
		{{0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}},
		{{-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}},
		{{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}},
		{{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}}
	};
	GLuint indexes[] = {
		0, 2, 1,
		1, 2, 3
	};
	Armature * armature;
	Animation * animation;
	struct ArmatureBone bones[] = {
		{ATOM("root"), BONE_INDEX_NOT_FOUND, VECTOR3f(0.0f, 0.0f, 0.0f)},
		{ATOM("boneLower"), 0, VECTOR3f(0.0f, -1.0f, 0.0f)},
		{ATOM("boneUpper"), 0, VECTOR3f(0.0f, 1.0f, 0.0f)}
	};
	struct AnimationBoneKeyframe frame1Bones[] = {
		{1, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, QUATERNIONf_IDENTITY, {0.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
		{2, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, QUATERNIONf_IDENTITY, {0.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}}
	};
	struct AnimationBoneKeyframe frame2Bones[] = {
		{1, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, QUATERNIONf_IDENTITY, {0.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
		{2, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, QUATERNIONf_IDENTITY, {0.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}}
	};
	struct AnimationKeyframe keyframes[] = {
		{1.0f, sizeof(frame1Bones) / sizeof(frame1Bones[0]), frame1Bones},
		{0.5f, sizeof(frame2Bones) / sizeof(frame2Bones[0]), frame2Bones}
	};
	AnimationState * animationState;
	
	if (renderer != NULL) {
		Renderer_dispose(renderer);
	}
	renderer = Renderer_create();
	Renderer_setClearColor(renderer, COLOR4f(0.25f, 0.0f, 0.125f, 0.0f));
	Renderer_setLights(renderer, VECTOR3f(0.0f, 8.0f, 8.0f), COLOR4f(1.0f, 1.0f, 0.95f, 1.0f), VECTOR3f(-1.0f, -2.0f, -8.0f), COLOR4f(0.8f, 0.8f, 0.8f, 1.0f), COLOR4f(0.1f, 0.1f, 0.105f, 1.0f));
	Renderer_setProjectionMatrix(renderer, Matrix4x4f_perspective(MATRIX4x4f_IDENTITY, 60.0f, (float) viewWidth / (float) viewHeight, 0.5f, 100.0f));
	
	if (renderable != NULL) {
		MeshRenderable_dispose(renderable);
	}
	armature = Armature_create(sizeof(bones) / sizeof(bones[0]), bones);
	animation = Animation_create(ATOM("animation"), armature, sizeof(keyframes) / sizeof(keyframes[0]), keyframes);
	animationState = Animation_createAnimationStateAtTime(animation, 0.0);
	renderable = MeshRenderable_createAnimated(vertices, sizeof(vertices) / sizeof(struct vertex_p3f_t2f_n3f_c4f), indexes, sizeof(indexes) / sizeof(GLuint), NULL, animationState);
	AnimationState_dispose(animationState);
	Renderer_addRenderable(renderer, RENDER_LAYER_3D_OPAQUE, (Renderable *) renderable);
	animationStartTime = Shell_getCurrentTime();
	
	if (cameraController != NULL) {
		OrbitCameraController_dispose(cameraController);
	}
	cameraController = OrbitCameraController_create();
}

static void Target_keyDown(unsigned int charCode, unsigned int keyCode, unsigned int modifiers, bool isRepeat) {
	switch (keyCode) {
		case KEYBOARD_1:
			initScene1();
			Shell_redisplay();
			break;
		case KEYBOARD_2:
			initScene2();
			Shell_redisplay();
			break;
	}
}

static void Target_keyUp(unsigned int keyCode, unsigned int modifiers) {
}

static void Target_keyModifiersChanged(unsigned int modifiers) {
	shiftKeyDown = !!(modifiers & MODIFIER_SHIFT_BIT);
	controlKeyDown = !!(modifiers & MODIFIER_CONTROL_BIT);
}

static void Target_mouseDown(unsigned int buttonNumber, float x, float y) {
	Shell_setMouseDeltaMode(true);
}

static void Target_mouseUp(unsigned int buttonNumber, float x, float y) {
	Shell_setMouseDeltaMode(false);
}

static void Target_mouseMoved(float x, float y) {
}

static void Target_mouseDragged(unsigned int buttonMask, float x, float y) {
	if (shiftKeyDown) {
		if (controlKeyDown) {
			OrbitCameraController_offset(cameraController, 0.0f, 0.0f, -y);
		} else {
			OrbitCameraController_offset(cameraController, x, -y, 0.0f);
		}
		
	} else if (controlKeyDown) {
		OrbitCameraController_zoom(cameraController, y);
		
	} else {
		OrbitCameraController_rotate(cameraController, x, y);
	}
	Shell_redisplay();
}

static void Target_scrollWheel(int deltaX, int deltaY) {
}

static void Target_resized(unsigned int newWidth, unsigned int newHeight) {
	viewWidth = newWidth;
	viewHeight = newHeight;
	glViewport(0, 0, viewWidth, viewHeight);
	if (renderer != NULL) {
		Renderer_setProjectionMatrix(renderer, Matrix4x4f_perspective(MATRIX4x4f_IDENTITY, 60.0f, (float) viewWidth / (float) viewHeight, 0.5f, 100.0f));
	}
}

static void Target_backgrounded() {
}

static void Target_foregrounded() {
}

static void registerShellCallbacks() {
	Shell_drawFunc(Target_draw);
	Shell_resizeFunc(Target_resized);
	Shell_keyDownFunc(Target_keyDown);
	Shell_keyUpFunc(Target_keyUp);
	Shell_keyModifiersChangedFunc(Target_keyModifiersChanged);
	Shell_mouseDownFunc(Target_mouseDown);
	Shell_mouseUpFunc(Target_mouseUp);
	Shell_mouseMovedFunc(Target_mouseMoved);
	Shell_mouseDraggedFunc(Target_mouseDragged);
	Shell_scrollWheelFunc(Target_scrollWheel);
	Shell_backgroundedFunc(Target_backgrounded);
	Shell_foregroundedFunc(Target_foregrounded);
}

#if defined(STEM_PLATFORM_macosx)
void NSOpenGLTarget_configure(int argc, const char ** argv, struct NSOpenGLShellConfiguration * configuration) {
	configuration->windowTitle = "GLGraphics";
#elif defined(STEM_PLATFORM_iphonesimulator) || defined(STEM_PLATFORM_iphoneos)
void EAGLTarget_configure(int argc, char ** argv, struct EAGLShellConfiguration * configuration) {
#elif defined(STEM_PLATFORM_win32) || defined(STEM_PLATFORM_win64)
void WGLTarget_configure(void * instance, void * prevInstance, char * commandLine, int command, int argc, const char ** argv, struct WGLShellConfiguration * configuration) {
	configuration->windowTitle = "GLGraphics";
#elif defined(STEM_PLATFORM_linux32) || defined(STEM_PLATFORM_linux64)
void GLXTarget_configure(int argc, const char ** argv, struct GLXShellConfiguration * configuration) {
	configuration->windowTitle = "GLGraphics";
#else
void GLUTTarget_configure(int argc, const char ** argv, struct GLUTShellConfiguration * configuration) {
	configuration->windowTitle = "GLGraphics";
#endif
	configuration->displayMode.depthBuffer = true;
	registerShellCallbacks();
}

void Target_init() {
	chdir(Shell_getResourcePath());
	initScene1();
	Shell_mainLoop();
}
