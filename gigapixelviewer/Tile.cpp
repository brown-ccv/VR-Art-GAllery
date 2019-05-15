#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>

#include "Tile.h"
#include "common.h"
#include "TIFFTileHandler.h"
#include "Layer.h"



namespace GiGaPixelViewer
{
	Tile::Tile(TIFFTileHandler&  tiffFile, const Point& offset, Layer& parentLayer, int miMapLevel
		, int mipmapResLevel)
		:myTiffFileHandler(tiffFile)
		, myAlpha(1.0f)
		, myVisible(1)
		, myScale(1.0f)
		, myParentLayer(parentLayer)
	{

		Point cellsize;
		Point topLeft;
		myTiffFileHandler.cellSize(cellsize);
		myTiffFileHandler.topLeft(topLeft);


		//myMipMaps.reserve(tiffFile.mipmapLevels());
		//int j = 1;
	//	for (int i = myTiffFileHandler.mipmapLevels() - 1; i >= 0; i--) {
		//TIFFSetDirectory(myTiffFileHandler.tiff(), miMapLevel);
		
		myMipMap.loaded = false;
		myMipMap.texid = -1;
		myMipMap.meterspp = cellsize.x;

		//myMipMap.width = myTiffFileHandler.width(miMapLevel) / mipmapResLevel;
		//myMipMap.height = myTiffFileHandler.height(miMapLeavel) / mipmapResLevel;
		
		TIFFSetDirectory(myTiffFileHandler.tiff(), miMapLevel);
		TIFFGetField(myTiffFileHandler.tiff(), TIFFTAG_TILEWIDTH, &myMipMap.width);
		TIFFGetField(myTiffFileHandler.tiff(), TIFFTAG_TILELENGTH, &myMipMap.height);

		myMipMap.offsetx = offset.x;
		myMipMap.offsety = offset.y;
		myMipMap.depth = miMapLevel;

		

		//	j = j * 2;
		//}

		myUpperLeft.x = topLeft.a + offset.x*cellsize.x;
		myUpperLeft.y = topLeft.b - offset.y*cellsize.y;
		myLowerRight.x = myUpperLeft.x + myTiffFileHandler.width(miMapLevel)*cellsize.x;
		myLowerRight.y = myUpperLeft.y - myTiffFileHandler.height(miMapLevel)*cellsize.y;

		myCenter.x = (myUpperLeft.x + myLowerRight.x) / 2.0;
		myCenter.y = (myUpperLeft.y + myLowerRight.y) / 2.0;
		myCenter.z = 0;

	}


	Tile::Tile(const Tile &obj) :myTiffFileHandler(obj.myTiffFileHandler)
		, myAlpha(obj.alpha())
		, myVisible(obj.visible())
		, myScale(obj.scale())
		, myParentLayer(obj.myParentLayer)
	{

	}

	Tile::~Tile()
	{
		//for (int i = 0; i < myMipMaps.size(); ++i)
		//{
		if (myMipMap.loaded)
		{
			glDeleteTextures(1, (GLuint*)&myMipMap.texid);
		}
			//fprintf(stderr, "Unloading %s layer %d unloaded\n", mipmaps[layer_number].name, layer_number);
			//myMipMaps[i].texid = -1;
		//}
		
	}

	Tile& Tile::operator=(const Tile &obj)
	{
		this->myTiffFileHandler = obj.myTiffFileHandler;
		this->myAlpha = obj.myAlpha;
		this->myScale = obj.myScale;
		this->myVisible = obj.myVisible;
	

		return *this;
	}

	float Tile::alpha() const
	{
		return myAlpha;
	}

	void Tile::setAlpha(float val)
	{
		myAlpha = val;
	}

	float Tile::scale() const
	{
		return myScale;
	}

	void Tile::setScale(float val)
	{
		myScale = val;
	}

	int Tile::visible() const
	{
		return myVisible;
	}

	void Tile::setVisible(int val)
	{
		myVisible = val;
	}

	const Point& Tile::center() const
	{
		return myCenter;
	}

	void Tile::setCenter(Point& val)
	{
		myCenter = val;
	}

	int Tile::tileTextureId(int mipMapIndex)
	{
		
		if (!myMipMap.loaded)
		{
			return 0;
		}
			
		return myMipMap.texid;

	}

	bool Tile::loadTexture(int mipMapIndex, unsigned char** memoryBlock)
	{
		//ptiffMipMap& tiffMipMap = myMipMaps[mipMapIndex];
		if (!myMipMap.loaded)
		{

			TIFFSetDirectory(myTiffFileHandler.tiff(), mipMapIndex);
			// every directory has its own resolution
			// 0 - 512
			// 1 - 256
			// 2 - 128
			// 3 - 64
			// 4 - 32

		
			TIFFGetField(myTiffFileHandler.tiff(), TIFFTAG_TILEWIDTH, &myMipMap.width);
			TIFFGetField(myTiffFileHandler.tiff(), TIFFTAG_TILELENGTH, &myMipMap.height);
			//myTileCount = (myWidth / myTileWidth) * (myHeight / myTileHeight);


			if (TIFFReadTile(myTiffFileHandler.tiff(), *memoryBlock,
				myMipMap.offsetx, myMipMap.offsety, 0, 0) == -1)
			{
				fprintf(stderr, "Error Reading tile!!!  \n");
			}
			
			GLuint id;
			glGenTextures(1, &id);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glBindTexture(GL_TEXTURE_2D, id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			/*float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);*/
			//fprintf(stderr, "Loading Texture %d\n", _mipmaps[layer_number].texid);
			//fprintf(stderr, "Size %d %d\n", _mipmaps[layer_number].width, _mipmaps[layer_number].height);

			/* Build mipmaps for the texture image.  Since we are not scaling the image
			(we easily could by calling glScalef), creating mipmaps is not really
			useful, but it is done just to show how easily creating mipmaps is. */
			switch (myTiffFileHandler.samplesPerPixel(myMipMap.depth)) {
			case 4:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, myMipMap.width,
					myMipMap.height, 0, GL_RGBA,
					GL_UNSIGNED_BYTE, *memoryBlock); break;
			case 3:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, myMipMap.width,
					myMipMap.height, 0, GL_RGB,
					GL_UNSIGNED_BYTE, *memoryBlock); break;
			case 1:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, myMipMap.width,
					myMipMap.height, 0, GL_LUMINANCE,
					GL_UNSIGNED_BYTE, *memoryBlock); break;
			}
			myMipMap.texid = id;
			myMipMap.loaded = true;

			return true;

		}

		return false;
	}

	bool Tile::isLoaded(int mipMapIndex)
	{
		return myMipMap.loaded;
	}

	void Tile::freetexture(int mipMapIndex)
	{
		
		if (myMipMap.loaded)
		{
			glDeleteTextures(1, &myMipMap.texid);
			myMipMap.loaded = false;
		}
		
	}

	int Tile::tileWidth(int mipMapIndex)
	{
		return myMipMap.width;
	}

	int Tile::tileHeight(int mipMapIndex)
	{
		return myMipMap.height;
	}


	const glm::vec3& Tile::screenCorrdinates() 
	{ 
		return myScreenCorrdinates; 
	}
	
	void Tile::setScreenCorrdinates(glm::vec3& val)
	{
		myScreenCorrdinates = val; 
	}

	const glm::vec2& Tile::worldCoodinates()
	{ 
		return myWorldCoodinates; 
	}

	void Tile::setWorldCoodinates(glm::vec2& val)
	{
		myWorldCoodinates = val; 
	}
}