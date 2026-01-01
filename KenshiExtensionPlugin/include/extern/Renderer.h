#pragma once
#include <ogre/OgrePrerequisites.h>
#include <kenshi/util/lektor.h>
#include <kenshi/util/OgreUnordered.h>
#include <ogre/OgreIdString.h>

class RenderEntityT;
class TitleScreen;
class MeshDataLookup;

// KenshiLibでは未定義
class TerrainDecalsManager
{
public:
	char _0x0[0x1e8];

	virtual ~TerrainDecalsManager();
};

// KenshiLibでは未定義
class RendererT
{
	lektor<RenderEntityT*> entities;

	virtual ~RendererT();
};

// KenshiLibでは未定義
class Renderer : public RendererT
{
public:
	TerrainDecalsManager* terrainDecalsManager;
	size_t _0x28;
	size_t _0x30[5];
	Ogre::Camera* mainCamera;
	Ogre::SceneManager* sceneManager;
	Ogre::Root* root;
	Ogre::Viewport* viewport;
	Ogre::RenderWindow* renderWindow;
	size_t _0x80;
	Ogre::CompositorWorkspace* compositorWorkspace;
	size_t _0x90;

	virtual ~Renderer();
};

//placeholder name
class CLASS_02130220
{
public:
	int _0x0;
	Renderer* renderer;
	void* cameraManager;
	void* _0x18;
	TitleScreen* title;
	ogre_unordered_map<Ogre::IdString, MeshDataLookup*> _0x28;
};
