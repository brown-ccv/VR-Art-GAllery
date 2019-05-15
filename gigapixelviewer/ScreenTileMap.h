#ifndef QUADBUFFER_H
#define QUADBUFFER_H
#include "common.h"
#include "ShaderProgram/ShaderProgram.h"


const float NUMTILES_X = 52.0f;
const float NUMTILES_Y = 13.0f;

const float NUM_TILES = 13.0f;

const int INT_NUMTILES_X = 13;
const int INT_NUMTILES_Y = 13;

namespace GiGaPixelViewer
{
	class FBO;
	class Tile;
	class Layer;
	class ScreenTileMap
	{
	  public:
		ScreenTileMap(int deviceWidth, int deviceHeight, int fboWidth, int fboHeight, Layer& layer);
		~ScreenTileMap();

		void Draw( int* viewport);

		int deviceHeight() const;
		void setDeviceHeight(int val); 


		int deviceWidth() const;
		void setDeviceWidth(int val);

		glm::vec3 getTileXYCoordinates(int x, int y);

		unsigned int minTileX() const; 
		void setMinTileX(unsigned int val); 


		unsigned int maxTileX() const; 
		void setMaxTileX(unsigned int val); 

		void moveTiles(int direction);

		unsigned int currentMipMap() const; 
		void setCurrentMipMap(unsigned int val); 

		float zoomFactor() const;
		void setZoomFactor(float val);

		float deviceScaleFactor() const; 
		void setDeviceScaleFactor(float val);

		void zoomIn(glm::vec2 mousePos);
		void zoomOut(glm::vec2 mousePos);

		void zoomIn( float val);
		void zoomOut(float val);

		glm::vec2 getTileCoordinates(glm::vec2& deviceCoodiantes, float zoomFactor);

		void setMouseOnTileScreenSpace(glm::vec2& mouseCoordinates);
		
		//void setMouseOnTileScreenSpace(glm::vec2& mouseCoordinates);

	

	private:
		
		 ShaderProgram renderToQuadShader;
		 int myDeviceHeight;
		 int myDeviceWidth;

		 int myFBOWidth;
		 int myFBOHeight;

		 /*render to quad*/
		 unsigned int myVAO;
		 GLuint myVBO;
		 glm::mat4 modelMatrix;
		 glm::mat4 scale;

		 glm::vec2 myBottonLeftTile;
		 glm::vec2 myBottomCenterTile;

		 Layer& myLayer;

		 unsigned int myMinTileX;
		 unsigned int myMaxTileX;

		 unsigned int myCurrentMipMap;

		 float myDeviceScaleFactor;
		 float myZoomFactor;
		 glm::vec2 myZoomWindowsCoordinates;
		 int myMaxXTilesPerViewPort;
		 int myMaxYTilesPerViewPort;
		 
		 glm::vec4 myMapArea;
		 glm::vec4 myWindowArea;

		 glm::vec2 myMouseOnTile;
		 

	};

}


#endif