#ifndef TIFFTILEHANDLER_H
#define TIFFTILEHANDLER_H

#include "TIFFHandler.h"

 

namespace GiGaPixelViewer
{
	struct Point;


	struct tileSize
	{
		int width;
		int height;
	};

	class TIFFTileHandler: public TIFFHandler
	{
	public:
		TIFFTileHandler();
		TIFFTileHandler(const TIFFTileHandler &obj);
		TIFFTileHandler& operator=(const TIFFTileHandler &obj);
		~TIFFTileHandler();
		int virtual openTIFF(const std::string& file);

		int tileWidth(int mimapLevel);
		int tileHeight(int mimapLevel);
		

		int tileCount();
		int mipmapLevels();

		bool cellSize(Point& cellSize) const;
		bool topLeft(Point& topLeft) const ;

		
		int mipmapTileWidth(int mipMap);
		int mipmapTileHeight(int mipMap);
		

	private:

		const ttag_t TIFFTAG_CELLSIZE = 33550;
		const ttag_t TIFFTAG_TOPLEFT = 33922;

		int myTileWidth;
		int myTileHeight;
		int myTileCount;
		int myMipmapLevels;

		std::vector<tileSize> tileSizesPerDirectory;
	};
}
#endif