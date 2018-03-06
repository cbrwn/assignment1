#include <iostream>

#include "imagemanager.h"

ImageManager::ImageManager() {}

ImageManager::~ImageManager()
{
	// destroy all textures
	for (auto t : m_textures)
	{
		aie::Texture* tex = t.second;
		if (tex != nullptr)
			delete tex;
	}
}

// gets a texture by name, and if it doesn't exist, tries to load it and 
// store it into our map
aie::Texture* ImageManager::getTexture(char* name)
{
	const char* fileNameTemplate = "./textures/%s.png";

	aie::Texture* loaded = m_textures[name];
	if (loaded == nullptr)
	{
		const int fileNameSize = 512;
		// texture doesn't exist - try loading
		char fileName[fileNameSize];
		sprintf_s(fileName, fileNameSize, fileNameTemplate, name);
		aie::Texture* tex = new aie::Texture(fileName);
		if (tex != nullptr)
		{
			m_textures[name] = tex;
			return tex;
		}
		else
		{
			// wasn't found!
			printf("Texture named %s was not found!\n", name);
			return nullptr;
		}
	}
	else
	{
		return loaded;
	}
}