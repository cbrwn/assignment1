#pragma once

#include <map>
#include <string>

#include "Texture.h"

class ImageManager
{
public:
	ImageManager();
	~ImageManager();

	//------------------------------------------------------------------------
	// Gets a texture with a specified name
	// If the texture has been loaded before, get that
	// If not, load the texture and store it for future use
	//
	// Param: 
	//			name: the name of the texture to get
	// Return: 
	//			pointer to the texture - nullptr if no texture could be loaded
	//------------------------------------------------------------------------
	aie::Texture* getTexture(char* name);
private:
	// where textures are stored
	std::map<std::string, aie::Texture*> m_textures;
};