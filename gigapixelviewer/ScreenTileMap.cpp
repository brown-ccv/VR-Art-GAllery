
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  

#include "ScreenTileMap.h"
#include "FBO.h"
#include "Tile.h"
#include "Layer.h"

namespace GiGaPixelViewer
{
	ScreenTileMap::ScreenTileMap(int deviceWidth, int deviceHeight, int fboWidth, int fboHeight, Layer& layer):
		myDeviceWidth(deviceWidth),myDeviceHeight(deviceHeight), myLayer(layer), myMinTileX(0),
		myMaxTileX(INT_NUMTILES_X), myCurrentMipMap(4), myDeviceScaleFactor(0.5f), myZoomFactor(1.f),
		myBottonLeftTile(glm::vec2(0,0)), myFBOWidth(fboWidth),myFBOHeight(fboHeight)
	{

		float quadVertices[] = {
			// positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			1.0f, -1.0f,  1.0f, 0.0f,
			1.0f,  1.0f,  1.0f, 1.0f
		};

		glGenVertexArrays(1, &myVAO);
		glGenBuffers(1, &myVBO);
		glBindVertexArray(myVAO);
		glBindBuffer(GL_ARRAY_BUFFER, myVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


		renderToQuadShader.LoadShaders("Shaders/renderToQuad.vert", "Shaders/renderToQuad.frag");
		renderToQuadShader.addUniform("screenTexture");
		renderToQuadShader.addUniform("tileCoordinates");
		renderToQuadShader.addUniform("M");
		renderToQuadShader.addUniform("secondHalf");
		renderToQuadShader.addUniform("highLight");
		renderToQuadShader.addUniform("centerTile");
		

		float tileScaleX = (1.0f / NUM_TILES) * myZoomFactor;
		float tileScaleY = (1.0f / NUM_TILES) * myZoomFactor;
		scale = glm::scale(glm::mat4(), glm::vec3(tileScaleX, tileScaleY, 0));	
		
		myWindowArea.x = -1; //bottom
		myWindowArea.y = -1; //left
		myWindowArea.z = 1;  //right
		myWindowArea.w = 1;  //top
	}


	ScreenTileMap::~ScreenTileMap()
	{
		if (myVAO != 0)glDeleteBuffers(1, &myVAO);
	}

	void ScreenTileMap::Draw(int* viewport)
	{
		
		renderToQuadShader.setUniformi("screenTexture", 0);
		//renderToQuadShader.setUniform("M",modelMatrix);

		renderToQuadShader.start();

		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		GLint viewP1x = viewport[0];
		GLint viewP1y = 0;
		GLint viewP1Width = viewport[2] * 0.5f;
		GLint viewP1Height = viewport[3];
		glViewport(viewP1x, viewP1y, viewP1Width, viewP1Height);
		//glViewport(0, 0, 1024, 1024);
		glBindVertexArray(myVAO);
		glActiveTexture(0);
		
		float sizeTilesPerViewport = (1.0f / NUM_TILES); 
		float tileScaleX = sizeTilesPerViewport * myZoomFactor;
		float tileScaleY = sizeTilesPerViewport * myZoomFactor;

	    myMaxXTilesPerViewPort = (int)((1.0f / tileScaleX) + 0.5f); // round value
		myMaxYTilesPerViewPort = (int)((1.0f / tileScaleY) + 0.5f); // round value

		float posX = 0.0f;
		float posY = 0.0f;
		
		glm::vec3 lastXpositon;
		int lastXpositonInMemory;
		
		renderToQuadShader.setUniformi("secondHalf",0);

		for (int tileX= 0; tileX <= myMaxXTilesPerViewPort; ++tileX)
		{
			
			posX = (-1.f + ( tileScaleX  * 2.f  *(float)tileX)); // tileScaleX * 2  because the size of the quad is 2 ( -1 to 1 counts 2 units)
			
			
			for (int tileY = 0; tileY < NUM_TILES ; ++tileY)
			{
				if (tileY + myBottonLeftTile.y < NUM_TILES)
				{
					Tile& tile = myLayer.getTile(myCurrentMipMap, tileX + myBottonLeftTile.x + myMinTileX, tileY + myBottonLeftTile.y);

					posY = (-1.0f + (tileScaleY * 2.f * (float)tileY));

					glm::vec3 screenCoordinates(posX + (tileScaleX * 0.5f), posY + (tileScaleY * 0.5f), 0);
					tile.setScreenCorrdinates(screenCoordinates);
					tile.setWorldCoodinates(glm::vec2(tileX + myMinTileX, tileY));

					glm::mat4 trans = glm::translate(glm::mat4(), screenCoordinates);
					scale = glm::scale(glm::mat4(), glm::vec3(tileScaleX, tileScaleY, 0));
					modelMatrix = trans * scale;
					renderToQuadShader.setUniform("M", modelMatrix);

					if (myMouseOnTile.x == tileX && myMouseOnTile.y == tileY)
					{
						renderToQuadShader.setUniformi("highLight", 1);
					}
					else
					{
						renderToQuadShader.setUniformi("highLight", 0);
					}

					glm::vec2 centerTile = getTileCoordinates(glm::vec2(0.f, 0.f), myZoomFactor);
					if (centerTile.x == tileX && centerTile.y == tileY)
					{
						renderToQuadShader.setUniformi("centerTile", 1);
					}
					else
					{
						renderToQuadShader.setUniformi("centerTile", 0);
					}


					if (!tile.isLoaded(myCurrentMipMap))
					{
						tile.loadTexture(myCurrentMipMap, myLayer.memoryBLock(myCurrentMipMap));
					}
					int textId = tile.tileTextureId(myCurrentMipMap);

					glBindTexture(GL_TEXTURE_2D, textId);
					glDrawArrays(GL_TRIANGLES, 0, 6);

				}
			
			}
		}

		myMapArea.x = 0.00; //bottom
		myMapArea.y = 0.00; //left
		myMapArea.z = myMaxXTilesPerViewPort; //right
		myMapArea.w = NUMTILES_Y; //top

		GLint viewP2x = viewport[2]  * 0.5f;
		GLint viewP2y = 0;
		GLint viewP2Width = viewport[2] * 0.5f;
		GLint viewP2Height = viewport[3] ;
		glViewport(viewP2x - 1 , viewP2y, viewP2Width, viewP2Height);
		//glViewport(1024, 0, 1024, 1024);
		renderToQuadShader.setUniformi("secondHalf", 0);
			
		for (int tileX = 0; tileX < myMaxXTilesPerViewPort; ++tileX)
		{
			for (int tileY = 0; tileY < NUM_TILES; ++tileY)
			{
				if (tileY + myBottonLeftTile.y < NUM_TILES)
				{
					int xtile = ((myMaxXTilesPerViewPort)+myMinTileX) + tileX;

					Tile& tile = myLayer.getTile(myCurrentMipMap, xtile + myBottonLeftTile.x, tileY + myBottonLeftTile.y);

					float posX = (-1.f + (tileScaleX * 2.f * tileX)); // tileScaleX * 2  because the size of the quad is 2 ( -1 to 1 counts 2 units)
					float posY = (-1.f + (tileScaleY * 2.f * tileY));
					glm::vec3 screenCoordinates(posX + (tileScaleX * 0.5f), posY + (tileScaleY * 0.5f), 0);
					tile.setScreenCorrdinates(screenCoordinates);
					tile.setWorldCoodinates(glm::vec2(xtile, tileY));

					glm::mat4 trans = glm::translate(glm::mat4(), screenCoordinates);
					scale = glm::scale(glm::mat4(), glm::vec3(tileScaleX, tileScaleY, 0));
					modelMatrix = trans * scale;
					renderToQuadShader.setUniform("M", modelMatrix);

					if (myMouseOnTile.x == xtile && myMouseOnTile.y == tileY)
					{
						renderToQuadShader.setUniformi("highLight", 1);
					}
					else
					{
						renderToQuadShader.setUniformi("highLight", 0);
					}

					glm::vec2 centerTile = getTileCoordinates(glm::vec2(0.f, 0.f), myZoomFactor);
					if (centerTile.x == xtile && centerTile.y == tileY)
					{
						renderToQuadShader.setUniformi("centerTile", 1);
					}
					else
					{
						renderToQuadShader.setUniformi("centerTile", 0);
					}



					if (!tile.isLoaded(currentMipMap()))
					{
						tile.loadTexture(myCurrentMipMap, myLayer.memoryBLock(myCurrentMipMap));
					}
					int textId = tile.tileTextureId(myCurrentMipMap);

					glBindTexture(GL_TEXTURE_2D, textId);
					glDrawArrays(GL_TRIANGLES, 0, 6);
				}
				
			}
		}
		renderToQuadShader.stop();
		
	}

	int ScreenTileMap::deviceHeight() const
	{
		return myDeviceHeight;
	}

	void ScreenTileMap::setDeviceHeight(int val)
	{
		myDeviceHeight = val;
	}

	int ScreenTileMap::deviceWidth() const
	{
		return myDeviceWidth;
	}

	void ScreenTileMap::setDeviceWidth(int val)
	{
		myDeviceWidth = val;
	}


	glm::vec3 ScreenTileMap::getTileXYCoordinates( int x, int y)
	{
		glm::vec3 deviceSpace;
		
		float distanceUnitDeviceX = 1 / NUM_TILES;
		float distanceUnitDeviceY = 1 / NUM_TILES;
		deviceSpace.x = x * distanceUnitDeviceX;
		deviceSpace.y = y * distanceUnitDeviceY;
		deviceSpace.z = 0.0f;
		return deviceSpace;
	}



	unsigned int ScreenTileMap::minTileX() const
	{
		return myMinTileX;
	}

	void ScreenTileMap::setMinTileX(unsigned int val)
	{
		myMinTileX = val;
	}

	unsigned int ScreenTileMap::maxTileX() const
	{
		return myMaxTileX;
	}

	void ScreenTileMap::setMaxTileX(unsigned int val)
	{
		myMaxTileX = val;
	}

	void ScreenTileMap::moveTiles(int direction)
	{
		if (direction < 0)
		{
			if (myMinTileX > 0)
			{
				myMinTileX--;
				myMaxTileX--;
				if (myMaxTileX % 5 == 0)
				{
					// release used textures every 5 tiles outside the screen
					int range = myMaxTileX + 5;
					for (int x = myMaxTileX + 1; x < range; x++)
					{
						
						for (int y = 0; y < NUMTILES_Y; ++y)
						{
							int posInMem;
							Tile& tile = myLayer.getTile(myCurrentMipMap,x, y);
							tile.freetexture(myCurrentMipMap);
						}
					}
				}
			}
		}
		else if(direction > 0)
		{
			if (myMinTileX + 1 <= 231)
			{
				myMinTileX++;
				myMaxTileX++;

				if (myMinTileX % 5 == 0)
				{
					// release used textures every 5 tiles outside the screen
					int range = myMinTileX - 5;
					for (int x = myMinTileX - 1; x >= range; x--)
					{
					
						for (int y = 0; y < NUMTILES_Y; ++y)
						{
							int posInMem;
							Tile& tile = myLayer.getTile(myCurrentMipMap, x , y);
							tile.freetexture(myCurrentMipMap);
						}
					}
				}
			}
		}

	}

	unsigned int ScreenTileMap::currentMipMap() const
	{ 
		return myCurrentMipMap; 
	}

	void ScreenTileMap::setCurrentMipMap(unsigned int val)
	{ 
		myCurrentMipMap = val; 
	}

	float ScreenTileMap::zoomFactor() const
	{ 
		return myZoomFactor; 
	}
	
	void ScreenTileMap::setZoomFactor(float val)
	{ 
		myZoomFactor = val; 
	}

	float ScreenTileMap::deviceScaleFactor() const 
	{ 
		return myDeviceScaleFactor; 
	}

	void ScreenTileMap::setDeviceScaleFactor(float val) 
	{ 
		myDeviceScaleFactor = val; 
	}


	void ScreenTileMap::zoomIn(float val)
	{
		myZoomFactor += val;
	}

	void ScreenTileMap::zoomOut(float val)
	{
		float temp = myZoomFactor - val;
		if (temp <= 1.0f)
		{
			myZoomFactor = 1.0f;
		}
		else
		{
			myZoomFactor -= val;
		}
	}


	void ScreenTileMap::zoomIn(glm::vec2 mousePos)
	{
		// before zoom
		glm::vec2 gridTileBefore = getTileCoordinates(mousePos, myZoomFactor);
		glm::vec2 centerTileBefore = getTileCoordinates(glm::vec2(0.f, 0.f), myZoomFactor);

		glm::vec2 offset1 = gridTileBefore - centerTileBefore;
		//std::cout << "centerTileBefore: " << centerTileBefore.x << " " << centerTileBefore.y << std::endl;

		glm::vec2 gridTileAfter = getTileCoordinates(mousePos, myZoomFactor + 0.1f);
		glm::vec2 centerTileAfter = centerTileBefore + (gridTileBefore - gridTileAfter);
		glm::vec2 offset2 = gridTileAfter - centerTileAfter;

		//std::cout << "centerTileAfter: " << centerTileAfter.x << " " << centerTileAfter.y << std::endl;

		float sizeTilesPerViewport = (1.0f / NUM_TILES);

		float tileScaleXBefore = sizeTilesPerViewport * (myZoomFactor);
		int localMaxXTilesPerViewPortBefore = (int)((1.0f / tileScaleXBefore) + 0.5f); // round value

		float tileScaleXAfter = sizeTilesPerViewport * (myZoomFactor + 0.1);
		int localMaxXTilesPerViewPortAfter = (int)((1.0f / tileScaleXAfter) + 0.5f); // round value

         
		//float newBottomLeft = localMaxXTilesPerViewPortAfter - centerTileAfter.x;
		if (centerTileBefore == centerTileAfter)
		{
			//if (gridTileBefore == gridTileAfter && gridTileBefore != centerTileAfter)
			if (gridTileBefore == gridTileAfter )
			{
				//myBottonLeftTile.x += (localMaxXTilesPerViewPortBefore - localMaxXTilesPerViewPortAfter);
				//myBottonLeftTile.y += (localMaxXTilesPerViewPortBefore - localMaxXTilesPerViewPortAfter);
			}
			/*else if (gridTileBefore == gridTileAfter && gridTileBefore == centerTileAfter)
			{
				std::cout << "Keep it in the center " << std::endl;
				myBottonLeftTile.x = localMaxXTilesPerViewPortAfter - centerTileAfter.x;
				myBottonLeftTile.y = localMaxXTilesPerViewPortAfter - centerTileAfter.y;
			}*/
			else if ( localMaxXTilesPerViewPortBefore != localMaxXTilesPerViewPortAfter)
			{
				myBottonLeftTile.x += (localMaxXTilesPerViewPortBefore - localMaxXTilesPerViewPortAfter);
				myBottonLeftTile.y += (localMaxXTilesPerViewPortBefore - localMaxXTilesPerViewPortAfter);
			}
			
		}
		else
		{
			myBottonLeftTile += (gridTileBefore - gridTileAfter);
		}
	
			
		myZoomFactor += 0.1;

	}

	void ScreenTileMap::zoomOut(glm::vec2 mousePos)
	{
		
		if (myZoomFactor <= 1.0f)
		{
			myZoomFactor = 1.f;
			myBottonLeftTile.y = 0;
			return;
		}

		float zoomFactor = myZoomFactor - 0.1 <= 1.f ? 0.f :  0.1;

		glm::vec2 gridTileBefore = getTileCoordinates(mousePos, myZoomFactor);
		glm::vec2 centerTileBefore = getTileCoordinates(glm::vec2(0.f, 0.f), myZoomFactor);

		std::cout << "centerTileBefore: " << centerTileBefore.x << " " << centerTileBefore.y << std::endl;

		glm::vec2 gridTileAfter = getTileCoordinates(mousePos, myZoomFactor - zoomFactor);
		glm::vec2 centerTileAfter = centerTileBefore + (gridTileBefore - gridTileAfter);

		std::cout << "centerTileAfter: " << centerTileAfter.x << " " << centerTileAfter.y << std::endl;

		float sizeTilesPerViewport = (1.0f / NUM_TILES);

		float tileScaleXBefore = sizeTilesPerViewport * (myZoomFactor);
		int localMaxXTilesPerViewPortBefore = (int)((1.0f / tileScaleXBefore) + 0.5f); // round value

		float tileScaleXAfter = sizeTilesPerViewport * (myZoomFactor - zoomFactor);
		int localMaxXTilesPerViewPortAfter = (int)((1.0f / tileScaleXAfter) + 0.5f); // round value


																					 //float newBottomLeft = localMaxXTilesPerViewPortAfter - centerTileAfter.x;
		if (centerTileBefore == centerTileAfter)
		{
			//if (gridTileBefore == gridTileAfter && gridTileBefore != centerTileAfter)
			if (gridTileBefore == gridTileAfter)
			{
				myBottonLeftTile.x += (localMaxXTilesPerViewPortBefore - localMaxXTilesPerViewPortAfter);
				myBottonLeftTile.y += (localMaxXTilesPerViewPortBefore - localMaxXTilesPerViewPortAfter);
			}
			/*else if (gridTileBefore == gridTileAfter && gridTileBefore == centerTileAfter)
			{
			std::cout << "Keep it in the center " << std::endl;
			myBottonLeftTile.x = localMaxXTilesPerViewPortAfter - centerTileAfter.x;
			myBottonLeftTile.y = localMaxXTilesPerViewPortAfter - centerTileAfter.y;
			}*/
			else if (localMaxXTilesPerViewPortBefore != localMaxXTilesPerViewPortAfter)
			{
				myBottonLeftTile.x += (localMaxXTilesPerViewPortBefore - localMaxXTilesPerViewPortAfter);
				myBottonLeftTile.y += (localMaxXTilesPerViewPortBefore - localMaxXTilesPerViewPortAfter);
			}

		}
		else
		{
			myBottonLeftTile += (gridTileBefore - gridTileAfter);
		}

		if (myBottonLeftTile.x < 0.f)
		{
			myBottonLeftTile.x = 0.f;
		}

		if (myBottonLeftTile.y < 0.f)
		{
			myBottonLeftTile.y = 0.f;
		}

		std::cout << "myBottonLeftTile: "<< myBottonLeftTile.x << " " << myBottonLeftTile.y <<std::endl;
		std::cout << std::endl;
		

		myZoomFactor -= 0.1;


	}
		
	glm::vec2 ScreenTileMap::getTileCoordinates(glm::vec2& deviceCoodiantes, float zoomFactor)
	{
		//int deviceRatio = myDeviceWidth / myDeviceHeight;
		//int fboRatio = myFBOWidth / myFBOHeight;

		int sizeRatio = myFBOWidth / myFBOHeight;

		float sizeTilesPerViewport = (1.0f / NUM_TILES);
		float tileScaleX = sizeTilesPerViewport *  zoomFactor;
		float tileScaleY = sizeTilesPerViewport *  zoomFactor;

		float localMaxXTilesPerViewPort = (int)((1.0f / tileScaleX) + 0.5f); // round value

		// Only in X
		localMaxXTilesPerViewPort *= sizeRatio;

		float backToOriginPosX = deviceCoodiantes.x + 1.f + (tileScaleX * 0.5f);
		float newTileX = (backToOriginPosX / (tileScaleX  * 2.f)) * sizeRatio;
		int i = (int)(  newTileX ) ;

		float backToOriginPosY = deviceCoodiantes.y + 1.f + (tileScaleY * 0.5f);
		float newTileY = backToOriginPosY / (tileScaleY  * 2.f);
		int j = (int)(newTileY);
		//std::cout << "cell i: " << i << " j: " << j << std::endl;

		return glm::vec2(i, j);


	}

	void ScreenTileMap::setMouseOnTileScreenSpace(glm::vec2& mouseCoordinates)
	{
		//std::cout << "myDeviceWidth: " << myDeviceWidth << " ,myDeviceHeight: " << myDeviceHeight << std::endl;
		//std::cout << "myFBOwidth: " << myFBOWidth << " ,myFBOHeight: " << myFBOHeight << std::endl;

		myMouseOnTile = getTileCoordinates(mouseCoordinates,myZoomFactor);
	}


}