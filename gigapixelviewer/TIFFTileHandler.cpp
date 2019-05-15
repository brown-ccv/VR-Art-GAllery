#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>

#include "TIFFTileHandler.h"
#include "Point.h"

#include <string>
#include <iostream>



namespace GiGaPixelViewer
{
	
	TIFFTileHandler::TIFFTileHandler()
		:myTileWidth(0)
		, myTileHeight(0)
		, myTileCount(0)
		, myMipmapLevels(0)
	{
	}


	TIFFTileHandler::TIFFTileHandler(const TIFFTileHandler &obj)
		:myTileWidth(obj.myTileWidth)
		, myTileHeight(obj.myTileHeight)
		, myTileCount(obj.myTileCount)
		, myMipmapLevels(obj.myMipmapLevels)
	{

	}

	TIFFTileHandler& TIFFTileHandler::operator=(const TIFFTileHandler &obj)
	{
		this->myTileWidth = obj.myTileWidth;
		this->myTileHeight = obj.myTileHeight;
		this->myTileCount = obj.myTileCount;
		this->myMipmapLevels = obj.myMipmapLevels;

		return *this;
	}

	TIFFTileHandler::~TIFFTileHandler()
	{
		if (myTiff)
		{
			TIFFClose(myTiff);
		}
		
	}

	int TIFFTileHandler::openTIFF(const std::string& file)
	{
		if (TIFFHandler::openTIFF(file))
		{
			// every directory has its own resolution
			// 0 - 512
			// 1 - 256
			// 2 - 128
			// 3 - 64
			// 4 - 32

			
			//myTileCount = (myWidth / myTileWidth) * (myHeight / myTileHeight);

		
			myMipmapLevels = myDircount;
			tileSizesPerDirectory = std::vector<tileSize>(myDircount, tileSize());


			for (int i = myDircount - 1; i >= 0; --i)
			{
				TIFFSetDirectory(myTiff, i);

				tileSize& size = tileSizesPerDirectory[i];
				TIFFGetField(myTiff, TIFFTAG_TILEWIDTH, &size.width);
				TIFFGetField(myTiff, TIFFTAG_TILELENGTH, &size.height);
			}


			std::cout << "Stats for " << file << "\n";
			std::cout << "Dir levels = " << myMipmapLevels << "\n";
		

			return true;
		}
		return false;
	}

	int TIFFTileHandler::tileWidth(int mimapLevel)
	{
		return myTileWidth;
	}

	int TIFFTileHandler::tileHeight(int mimapLevel)
	{
		return myTileHeight;
	}

	int TIFFTileHandler::tileCount()
	{
		return myTileCount;
	}

	int TIFFTileHandler::mipmapLevels()
	{
		return myMipmapLevels;
	}

	bool TIFFTileHandler::cellSize(Point& cellSize) const
	{
		int count = 0;
		double *dptr = 0;

		TIFFGetField(myTiff, TIFFTAG_CELLSIZE, &count, &dptr);

//		std::cout << count << std::endl;

		if (count == 3)
		{
			//memcpy(_cellsize, dptr, 3 * sizeof(double));
			cellSize.x = dptr[0];
			cellSize.y = dptr[1];
			cellSize.z = dptr[2];
		}
		else
		{
			return false;
		}


		if (cellSize.y < 0.0)
		{
			// C.D: This is legacy code. I have no idea what it does.
			cellSize.y = -cellSize.y;  // XXX - probably shouldn't do this! 
		}


		return true;
	}

	bool TIFFTileHandler::topLeft(Point& topLeft) const
	{
		int count;
		double *dptr;

		TIFFGetField(myTiff, TIFFTAG_TOPLEFT, &count, &dptr);

		if (count == 6)
		{
			//memcpy(_topleft, dptr, 6 * sizeof(double));
			topLeft.x = dptr[0];
			topLeft.y = dptr[1];
			topLeft.z = dptr[2];
			topLeft.a = dptr[3];
			topLeft.b = dptr[4];
			topLeft.c = dptr[5];
			return true;
		}

		return false;
	}

	
	

	int TIFFTileHandler::mipmapTileWidth(int mipMap)
	{
		return tileSizesPerDirectory[mipMap].width;
	}

	int TIFFTileHandler::mipmapTileHeight(int mipMap)
	{
		return tileSizesPerDirectory[mipMap].height;
	}

}


