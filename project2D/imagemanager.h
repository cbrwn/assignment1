#pragma once

#include <map>
#include <string>
#include <Texture.h>

class ImageManager
{
public:
	ImageManager();
	~ImageManager();

	aie::Texture* getTexture(char* name);
private:
	std::map<std::string, aie::Texture*> m_textures;
};