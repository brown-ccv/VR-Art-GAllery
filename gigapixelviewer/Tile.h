#ifndef TILE_H
#define TILE_H

#include "Point.h"
#include "common.h"
#include <vector>

class Texture;
namespace GiGaPixelViewer
{
	
	struct ptiffMipMap
	{
		ptiffMipMap() :width(0), height(0), offsetx(0), offsety(0), meterspp(0.0), depth(0),
			texid(0), loaded(false)
		{}
		int width;
		int height;
		int offsetx, offsety;
		double meterspp;
		int depth;
		GLuint texid;
		bool loaded;
	};
	
	class TIFFTileHandler;
	class Layer;
	class Tile
	{
	public:
		Tile( TIFFTileHandler&  tiffFile, const Point& point , Layer& parentLayer,int mimapLevel
		, int mipmapResLevel);
		Tile(const Tile &obj);
		~Tile();

		Tile& operator=(const Tile &obj);
		

		float alpha() const; 
		void setAlpha(float val);

		float scale() const; 
		void setScale(float val);


		int visible() const;
		void setVisible(int val); 


		const Point& center() const; 
		void setCenter(Point& val); 

		int tileTextureId(int mipMapIndex);

		bool loadTexture(int mipMapIndex, unsigned char** memoryBlock);
		bool isLoaded(int mipMapIndex);
		void freetexture(int mipMapIndex);

		int tileWidth(int mipMapIndex);
		int tileHeight(int mipMapIndex);
		

		const glm::vec3& screenCorrdinates() ; 
		void setScreenCorrdinates(glm::vec3& val); 

		const glm::vec2& worldCoodinates() ; 
		void setWorldCoodinates(glm::vec2& val); 

	private:
		float myAlpha;
		float myScale;
		int myVisible;
		Point myCenter;
		Point myUpperLeft;
		Point myLowerRight;
		
		ptiffMipMap myMipMap;
		TIFFTileHandler& myTiffFileHandler;
		Layer& myParentLayer;
		
		glm::vec3 myScreenCorrdinates; // ignore z component
		glm::vec2 myWorldCoodinates; // x , y tile coordinates
	};

}

#endif