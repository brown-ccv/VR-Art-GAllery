#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  

#include "Layer.h"
#include "TIFFTileHandler.h"
#include "Tile.h"
#include "ScreenTileMap.h"
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include "./Model/Texture.h"

namespace GiGaPixelViewer
{

	Layer::Layer():activeTile(0)
	{
	}


	Layer::~Layer()
	{
		delete myTexture;

		for (int i = 0 ; i < myMemoryBlocks.size(); i++)
		{
			delete myMemoryBlocks[i];
		}
		
	}

	void Layer::createLayer(const std::string& layerFileName)
	{
		
		std::ifstream infile(layerFileName);
		std::string type, line, path;
		float alpha = 0.f, scale = 0.f;
		while (std::getline(infile, line) )
		{
			std::istringstream stream(line);
			stream >> type >> path >> alpha >> scale;
			std::cout << type << std::endl;
			std::cout << path << std::endl;
			std::cout << alpha << std::endl;
			std::cout << scale << std::endl;
			bool opResult = buildLayer(path, alpha, scale);
			if (opResult)
			{
				std::cout << "Image " << path << "Loaded \n";
			}
			else
			{
				std::cout << "Image " << path << "not Loaded \n";
			}
		}
		
	}

	void Layer::setTilesAlpha(float alpha)
	{
		/*for (int i = 0 ; i < myTiles.size() ; ++i)
		{
			myTiles[i]->setAlpha(alpha);
		}*/
	}

	void Layer::setTilesScale(float scale)
	{
		/*for (int i = 0; i < myTiles.size(); ++i)
		{
			myTiles[i]->setScale(scale);
		}*/
	}

	void Layer::setTIlesVisible(int visible)
	{
		/*for (int i = 0; i < myTiles.size(); ++i)
		{
			myTiles[i]->setVisible(visible);
		}*/
	}

	int Layer::numTiles()
	{
		return myTiles.size();
	}

	const std::string& Layer::fileName() 
	{
		return myFileName;
	}

	void Layer::setFileName(const std::string& val)
	{
		myFileName = val;
	}

	bool Layer::buildLayer(const std::string& layerFileName, float alpha, float scale)
	{
		if (myTiffTileFile.openTIFF(layerFileName))
		{
			
			myTiles = std::vector<std::vector<Tile*>>(myTiffTileFile.mipmapLevels());
			myMemoryBlocks = std::vector<unsigned char*>(myTiffTileFile.mipmapLevels(), 0);
			int j = 1;
			for (int mipMaplvl = myTiffTileFile.mipmapLevels() - 1; mipMaplvl >= 0; --mipMaplvl)
			{
				if (myTiffTileFile.bitsPerSample(mipMaplvl) == 8)
				{

					Point cellsize;
					Point topLeft;

					myTiffTileFile.cellSize(cellsize);
					myTiffTileFile.topLeft(topLeft);

					myCenter.x = topLeft.a + cellsize.x * myTiffTileFile.width(mipMaplvl) / 2;
					myCenter.y = topLeft.b - cellsize.y * myTiffTileFile.height(mipMaplvl) / 2;
					myCenter.z = 0;


					int maxTileWidth = 0;
					int maxTileHeight = 0;
					std::vector<Tile*> v;


					int tifWidth = myTiffTileFile.width(mipMaplvl);
					int tifHeight = myTiffTileFile.height(mipMaplvl);

					int tileWidth = myTiffTileFile.mipmapTileWidth(mipMaplvl);
					int tileHeight = myTiffTileFile.mipmapTileHeight(mipMaplvl);

					std::cout << "mipMaplvl: " << mipMaplvl << std::endl;
					std::cout << "num tiles : "<< (tifWidth / tileWidth) * (tifHeight / tileHeight) << std::endl;

					int numTiles = 0;
					for (int x = 0; x < tifWidth; x += tileWidth) {
						for (int y = 0; y < tifHeight; y += tileHeight) {
							Point offset(x, y);
							Tile* newtile = new Tile(myTiffTileFile, offset, *this, mipMaplvl, j);

							maxTileWidth = newtile->tileWidth(mipMaplvl) > maxTileWidth ? newtile->tileWidth(mipMaplvl) : maxTileWidth;
							maxTileHeight = newtile->tileHeight(mipMaplvl) > maxTileHeight ? newtile->tileHeight(mipMaplvl) : maxTileHeight;

							v.push_back(newtile);

							numTiles++;
						}
					}

					std::cout << "num tiles 2: " << numTiles << std::endl;

					j = j * 2;
					allocSize = sizeof(unsigned char)
						*maxTileWidth*maxTileHeight* myTiffTileFile.samplesPerPixel(mipMaplvl);


					unsigned char* memBlock = new unsigned char[allocSize];
					myMemoryBlocks[mipMaplvl] = memBlock;
					myTiles[mipMaplvl]= v;

				}
				else
				{
					std::cout << "Error with " << layerFileName <<
						" Can only read 8 bit at the moment " << myTiffTileFile.samplesPerPixel(mipMaplvl) << "\n";
					assert(false);
				}
			}

			return true;
			
			
		}

		return false;
	}

	//void Layer::draw(QuadBuffer& quadBuffer)
	//{
	//	
	//	quadBuffer.Draw(0);
	//	
	//	/*for (int i=0; i <myTiles.size();++i)
	//	{
	//		quadBuffer.Draw(myTiles[i]->tileTextureId(4));
	//	}*/

	//	/*if (!myTexture)
	//	{
	//		myTexture = new Texture(GL_TEXTURE_2D, "dragonballzsuper.jpg");
	//		
	//	}
	//	quadBuffer.Draw(myTexture->GetTextureId());*/
	//	
	//	
	//}

	Tile& Layer::getTile(int mipMap, int x, int y)
	{
		
		int pos = INT_NUMTILES_X  * x   + INT_NUMTILES_Y - 1 - y;

		if (pos < 0)
		{
			return *myTiles[mipMap][0];
		}
		else if (pos > myTiles[mipMap].size())
		{
			int lastPos = myTiles[mipMap].size();
			return *myTiles[mipMap][lastPos - 1];
		}
		else
		{
			return *myTiles[mipMap][pos];
		}//int pos = y + x * INT_NUMTILES_Y;
		
		
	}

	int Layer::samplesPerPixel()
	{
		return myTiffTileFile.samplesPerPixel(0);
	}

	unsigned char** Layer::memoryBLock(int mipMap)
	{
		return &myMemoryBlocks[mipMap];
	}

	void Layer::clearMemoryBLock(int mipMap)
	{
		memset(myMemoryBlocks[mipMap], 0, sizeof(unsigned char) * allocSize);
	}

	TIFFTileHandler& const Layer::tifHandler()
	{
		return  myTiffTileFile;
	}

}