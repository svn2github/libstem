#include "3dmodelio/MaterialData.h"
#include "3dmodelio/MeshData.h"
#include "3dmodelio/Obj3DModelIO.h"
#include "binaryserialization/BinaryDeserializationContext.h"
#include "gamemath/Matrix4x4f.h"
#include "glgraphics/GLIncludes.h"
#include "glgraphics/MeshRenderable.h"
#include "glgraphics/OrbitCamera.h"
#include "glgraphics/Renderer.h"
#include "jsonserialization/JSONDeserializationContext.h"
#include "pngimageio/PNGImageIO.h"
#include "resourcemanager/FileCatalog.h"
#include "resourcemanager/ResourceManager.h"
#include "shell/Shell.h"
#include "shell/ShellCallbacks.h"
#include "shell/ShellKeyCodes.h"
#include "utilities/IOUtilities.h"

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

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define PROJECTION_FOV 60.0f

static ResourceManager * resourceManager;
static FileCatalog * fileCatalog;
static char * fileCatalogPath;
static VertexBuffer * vertexBuffer;
static MeshRenderable * renderable;
static Material * material;
static Renderer * renderer;
static OrbitCamera * camera;
static bool shiftKeyDown, controlKeyDown;
static unsigned int viewWidth = 1280, viewHeight = 720;

static bool Target_draw() {
	Renderer_clear(renderer);
	Renderer_setViewMatrix(renderer, OrbitCamera_getMatrix(camera));
	Renderer_beginDrawing(renderer);
	Renderer_drawLayer(renderer, RENDER_LAYER_3D_OPAQUE);
	Renderer_endDrawing(renderer);
	return true;
}

static void useMesh(MeshData * meshData) {
	if (material != NULL) {
		Material_dispose(material);
		material = NULL;
	}
	
	Renderer_clearAllRenderables(renderer);
	if (renderable != NULL) {
		MeshRenderable_dispose(renderable);
	}
	if (vertexBuffer != NULL) {
		VertexBuffer_dispose(vertexBuffer);
	}
	
	if (meshData->materialName != NULL) {
		MaterialData * materialData;
		
		materialData = ResourceManager_referenceResource(resourceManager, ATOM("material"), meshData->materialName);
		if (materialData != NULL) {
			material = Material_create(materialData->color, materialData->specularity, materialData->shininess, materialData->emissiveness);
			if (materialData->colorMapName != NULL) {
				BitmapImage * colorImage;
				
				colorImage = ResourceManager_referenceResource(resourceManager, ATOM("png"), materialData->colorMapName);
				if (colorImage != NULL) {
					Material_setColorTexture(material, materialData->magnifyColorMapNearest, colorImage->width, colorImage->height, colorImage->pixels);
					ResourceManager_releaseResource(resourceManager, ATOM("png"), materialData->colorMapName);
				} else {
					fprintf(stderr, "Couldn't load color texture image \"%s\" for material \"%s\" in mesh \"%s\"\n", materialData->colorMapName, meshData->materialName, meshData->name);
				}
			}
			if (materialData->normalMapName != NULL) {
				BitmapImage * normalImage;
				
				normalImage = ResourceManager_referenceResource(resourceManager, ATOM("png"), materialData->normalMapName);
				if (normalImage != NULL) {
					Material_setNormalTexture(material, materialData->magnifyColorMapNearest, normalImage->width, normalImage->height, normalImage->pixels);
					ResourceManager_releaseResource(resourceManager, ATOM("png"), materialData->normalMapName);
				} else {
					fprintf(stderr, "Couldn't load normal texture image \"%s\" for material \"%s\" in mesh \"%s\"\n", materialData->normalMapName, meshData->materialName, meshData->name);
				}
			}
		} else {
			fprintf(stderr, "Couldn't load material \"%s\" for mesh \"%s\"\n", meshData->materialName, meshData->name);
		}
	}
	vertexBuffer = VertexBuffer_createPTNC(meshData->vertices, meshData->vertexCount, meshData->indexes, meshData->indexCount);
	renderable = MeshRenderable_create(vertexBuffer, material, NULL, MATRIX4x4f_IDENTITY);
	Renderer_addRenderable(renderer, RENDER_LAYER_3D_OPAQUE, (Renderable *) renderable);
	Shell_redisplay();
}

static DeserializationContext * guessDeserializationType(const char * filePath) {
	FILE * file;
	int firstChar;
	DeserializationContext * context;
	
	file = fopen(filePath, "rb");
	if (file == NULL) {
		fprintf(stderr, "Couldn't open \"%s\"\n", filePath);
		return NULL;
	}
	firstChar = fgetc(file);
	fclose(file);
	
	switch (firstChar) {
		case '{':
			context = (DeserializationContext *) JSONDeserializationContext_createWithFile(filePath);
			if (context->status != SERIALIZATION_ERROR_OK) {
				fprintf(stderr, "File \"%s\" looks like JSON, but JSONDeserializationContext couldn't parse it.\n", filePath);
				context = NULL;
			}
			break;
		case 'S':
		case 'm':
			context = (DeserializationContext *) BinaryDeserializationContext_createWithFile(filePath);
			break;
		default:
			fprintf(stderr, "Couldn't determine serialization type of file \"%s\" (first character was '%c')\n", filePath, firstChar);
			context = NULL;
			break;
	}
	return context;
}

static const char * filePathForResource(Atom resourceName, Atom type) {
	const char * filePath = NULL;
	
	if (fileCatalog != NULL) {
		filePath = FileCatalog_getFilePath(fileCatalog, type, resourceName);
	}
	if (filePath == NULL) {
		filePath = resourceName;
	}
	return filePath;
}

static void * deserializeFile(const char * filePath, void * (* deserializeFunction)(compat_type(DeserializationContext *))) {
	DeserializationContext * context;
	void * result;
	
	context = guessDeserializationType(filePath);
	if (context == NULL) {
		return NULL;
	}
	result = deserializeFunction(context);
	if (result == NULL) {
		fprintf(stderr, "Couldn't deserialize \"%s\" (error %d)", filePath, context->status);
		context->dispose(context);
		return NULL;
	}
	context->dispose(context);
	return result;
}

static void * loadMeshResource(Atom resourceName, void * context) {
	return deserializeFile(filePathForResource(resourceName, ATOM("mesh")), (void * (*)(void *)) MeshData_deserialize);
}

static void unloadMeshResource(void * resource, void * context) {
	MeshData_dispose(resource);
}

static void * loadMaterialResource(Atom resourceName, void * context) {
	return deserializeFile(filePathForResource(resourceName, ATOM("material")), (void * (*)(void *)) MaterialData_deserialize);
}

static void unloadMaterialResource(void * resource, void * context) {
	MaterialData_dispose(resource);
}

void * loadPNGResource(const char * resourceName, void * context) {
	return PNGImageIO_loadPNGFile(filePathForResource(resourceName, ATOM("png")), PNG_PIXEL_FORMAT_AUTOMATIC, true);
}

void unloadPNGResource(void * resource, void * context) {
	BitmapImage_dispose(resource);
}

static void loadFileCatalog(const char * filePath) {
	if (fileCatalog != NULL) {
		FileCatalog_dispose(fileCatalog);
	}
	if (fileCatalogPath != NULL) {
		free(fileCatalogPath);
		fileCatalogPath = NULL;
	}
	fileCatalog = deserializeFile(filePath, (void * (*)(void *)) FileCatalog_deserialize);
	
	if (fileCatalog != NULL) {
		size_t meshCount = 0;
		const char ** meshNames;
		
		FileCatalog_setBasePath(fileCatalog, getDirectory(filePath));
		
		meshNames = FileCatalog_listNamesForType(fileCatalog, ATOM("mesh"), &meshCount);
		if (meshCount > 0) {
			MeshData * meshData;
			
			meshData = ResourceManager_referenceResource(resourceManager, ATOM("mesh"), ATOM(meshNames[0]));
			if (meshData == NULL) {
				fprintf(stderr, "Couldn't load mesh \"%s\"\n", meshNames[0]);
			} else {
				useMesh(meshData);
				ResourceManager_releaseResource(resourceManager, ATOM("mesh"), ATOM(meshNames[0]));
			}
		} else {
			fprintf(stderr, "File catalog \"%s\" doesn't appear to contain any meshes; not displaying anything\n", filePath);
		}
		free(meshNames);
	}
}

static void loadObjFile(const char * filePath) {
	MeshData * meshData;
	
	meshData = Obj3DModelIO_loadFile("suzanne.obj");
	useMesh(meshData);
	MeshData_dispose(meshData);
}

static void Target_keyDown(unsigned int charCode, unsigned int keyCode, unsigned int modifiers, bool isRepeat) {
	switch (keyCode) {
		case KEYBOARD_O: {
			char filePath[PATH_MAX];
			
			if (Shell_openFileDialog(NULL, filePath, PATH_MAX)) {
				const char * fileExtension = getFileExtension(filePath);
				
				if (!strcmp(fileExtension, "catalog")) {
					loadFileCatalog(filePath);
					
				} else if (!strcmp(fileExtension, "obj")) {
					loadObjFile(filePath);
					
				} else if (!strcmp(fileExtension, "mesh")) {
					MeshData * meshData = deserializeFile(filePath, (void * (*)(void *)) MeshData_deserialize);
					if (meshData != NULL) {
						useMesh(meshData);
						MeshData_dispose(meshData);
					}
					
				} else {
					fprintf(stderr, "Didn't know how to handle file extension \"%s\"\n", fileExtension);
				}
			}
			break;
		}
		case KEYBOARD_F:
			OrbitCamera_frameBoundingBox(camera, renderable->vertexBuffer->bounds, PROJECTION_FOV, (float) viewWidth / (float) viewHeight);
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
			OrbitCamera_offset(camera, 0.0f, 0.0f, y);
		} else {
			OrbitCamera_offset(camera, -x, y, 0.0f);
		}
		
	} else if (controlKeyDown) {
		OrbitCamera_zoom(camera, y);
		
	} else {
		OrbitCamera_rotate(camera, x, y);
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
		Renderer_setProjectionMatrix(renderer, Matrix4x4f_perspective(MATRIX4x4f_IDENTITY, PROJECTION_FOV, (float) viewWidth / (float) viewHeight, 0.02f, 20.0f));
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
	configuration->windowTitle = "3DModelIO";
	configuration->useGLCoreProfile = true;
#elif defined(STEM_PLATFORM_iphonesimulator) || defined(STEM_PLATFORM_iphoneos)
void EAGLTarget_configure(int argc, char ** argv, struct EAGLShellConfiguration * configuration) {
#elif defined(STEM_PLATFORM_win32) || defined(STEM_PLATFORM_win64)
void WGLTarget_configure(void * instance, void * prevInstance, char * commandLine, int command, int argc, const char ** argv, struct WGLShellConfiguration * configuration) {
	configuration->windowTitle = "3DModelIO";
#elif defined(STEM_PLATFORM_linux32) || defined(STEM_PLATFORM_linux64)
void GLXTarget_configure(int argc, const char ** argv, struct GLXShellConfiguration * configuration) {
	configuration->windowTitle = "3DModelIO";
#else
void GLUTTarget_configure(int argc, const char ** argv, struct GLUTShellConfiguration * configuration) {
	configuration->windowTitle = "3DModelIO";
#endif
	configuration->displayMode.depthBuffer = true;
	registerShellCallbacks();
}

void Target_init() {
	chdir(Shell_getResourcePath());
	renderer = Renderer_create();
	Renderer_setClearColor(renderer, COLOR4f(0.0f, 0.125f, 0.25f, 0.0f));
	Renderer_setLights(renderer, VECTOR3f(0.0f, 8.0f, 8.0f), COLOR4f(1.0f, 1.0f, 0.95f, 1.0f), VECTOR3f(-1.0f, -2.0f, -8.0f), COLOR4f(0.8f, 0.8f, 0.8f, 1.0f), COLOR4f(0.1f, 0.1f, 0.105f, 1.0f));
	
	resourceManager = ResourceManager_create(Shell_getCurrentTime);
	ResourceManager_addTypeHandler(resourceManager, ATOM("mesh"), loadMeshResource, unloadMeshResource, PURGE_DEFERRED, NULL);
	ResourceManager_addTypeHandler(resourceManager, ATOM("material"), loadMaterialResource, unloadMaterialResource, PURGE_DEFERRED, NULL);
	ResourceManager_addTypeHandler(resourceManager, ATOM("png"), loadPNGResource, unloadPNGResource, PURGE_DEFERRED, NULL);
	
	loadObjFile("suzanne.obj");
	camera = OrbitCamera_create();
	
	Shell_mainLoop();
}
