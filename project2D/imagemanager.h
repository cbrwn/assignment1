#pragma once

#include <map>
#include <Texture.h>

class ImageManager
{
public:
	ImageManager();
	~ImageManager();

	aie::Texture* getTexture(char* name);
private:
	std::map<char*, aie::Texture*> m_textures;
};