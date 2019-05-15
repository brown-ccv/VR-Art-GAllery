#include "Texture.h"
#include <FreeImage\FreeImage.h>

#include <direct.h>  
#include <stdlib.h>  
#include <stdio.h>


Texture::Texture(GLenum TextureTarget, const std::string& filename) : myTextureTarget(TextureTarget)
, myFileName(filename.c_str())
{
	texture_id = Load2DTexture();
}
	
Texture::~Texture()
{
}

void Texture::Bind(GLenum TextureUnit)
{
	glActiveTexture(GL_TEXTURE0 + TextureUnit);
	glBindTexture(myTextureTarget, texture_id);
}

GLuint Texture::GetTextureId()
{
	return texture_id;
}

int Texture::Load2DTexture()
{
	

	int width = 0;
	int height = 0;
	int bbp = 0;
	//pointer to the image data
	BYTE * bits(0);

	//retrieve the image data
	//if this somehow one of these failed (they shouldn't), return failure
	if (!LoadTexture(myFileName, width, height, &bits, bbp))
	{
		assert(false && "Image failed to load 2");
	}

	unsigned int textureID = 0;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Nice trilinear filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	// Read the file, call glTexImage2D with the right parameters
	if (bbp == 24)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
			0, GL_BGR, GL_UNSIGNED_BYTE, bits);
	}
	else if (bbp == 32)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
			0, GL_BGRA, GL_UNSIGNED_BYTE, bits);
	}
	else
	{
		std::string message = "pixel size = " + std::to_string(bbp) + " don't know how to process this case. I'm out!";
		assert(false && message.c_str());
	}
	
	
	//glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Return the ID of the texture we just created
	return textureID;
}

bool Texture::LoadTexture(const std::string & fileName, int & width, int & height, BYTE ** data, int& bbp)
{

	//image format
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	//pointer to the image, once loaded
	FIBITMAP *dib(0);
	BYTE* bits(0);

	GLuint gl_texID;

	std::string resourcesTexturesFolderPath = "Resources/Textures/";
	std::string textureFullPath = resourcesTexturesFolderPath.append(fileName);

	//check the file signature and deduce its format
	fif = FreeImage_GetFileType(textureFullPath.c_str(), 0);
	//if still unknown, try to guess the file format from the file extension
	if (fif == FIF_UNKNOWN)
	{
		fif = FreeImage_GetFIFFromFilename(textureFullPath.c_str());
	}


	//if still unknown, return failure
	if (fif == FIF_UNKNOWN)
	{
		assert(false && "texture unknown file format");
	}


	//check that the plugin has reading capabilities and load the file
	if (FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, textureFullPath.c_str());

	

	height = FreeImage_GetHeight(dib);
	width = FreeImage_GetWidth(dib);
	bbp = FreeImage_GetBPP(dib);
	bits = FreeImage_GetBits(dib);
	*data = (BYTE *)malloc(width * height * (bbp / 8));
	memcpy(*data, bits, width * height * (bbp / 8));

	//if the image failed to load, return failure
	if (!dib)
	{
		assert(false && "Image failed to load 1");
	}

	//Free FreeImage's copy of the data
	FreeImage_Unload(dib);

	return true;
}
