#ifndef LAYER_H
#define LAYER_H
#include <vector>
#include "Point.h"
#include "TIFFTileHandler.h"


class Texture;
namespace GiGaPixelViewer
{
	class TIFFHandler;
	class Tile;
	class ScreenTileMap;
	class Layer
	{
	public:
		Layer();
		~Layer();
		enum LayerType { LAYER, IMAGE, SHAPE, GROUP };
		
		void createLayer(const std::string& layerFileName);

		void setTilesAlpha(float alpha);
		void setTilesScale(float scale);
		void setTIlesVisible(int visible);

		int numTiles();

		const std::string& fileName();
		void setFileName(const std::string& val); 
		//void draw(QuadBuffer& quadBuffer);

		Tile& getTile(int mipMap, int x, int y);

		int samplesPerPixel();

		unsigned char** memoryBLock(int mipMap);

		void clearMemoryBLock(int mipMap);

		TIFFTileHandler& const tifHandler();

	protected:
		bool buildLayer(const std::string& layerFileName , float alpha, float scale);
		


	private:
	
		Point myCenter;

		double meterspp;
		char _name[80];
		int _group;
		LayerType type;
		float gamma, gmin, gmax;
		double _topleft[6];
		double _cellsize[3];

		std::vector<std::vector<Tile*>> myTiles;
		std::string myFileName;
		int activeTile;
		

	private:
		TIFFTileHandler myTiffTileFile;
		Texture* myTexture;
		std::vector<unsigned char*> myMemoryBlocks;

		size_t allocSize;
	};
}

#endif