#ifndef WALLMESH_H
#define WALLMESH_H

#include "common.h"
class ShaderProgram;
class Texture;
namespace GiGaPixelViewer
{
	class FBO;

	struct Triangle
	{
		glm::vec3 vertex0;
		glm::vec3 vertex1;
		glm::vec3 vertex2;
		
	};

	class WallMesh
	{
	public:
		WallMesh();
		~WallMesh();
		void draw(ShaderProgram& shader);
		void draw(ShaderProgram& shader, FBO& fbo);

		void drawControlPoints(ShaderProgram& shader);

		glm::vec3 position() const;
		void setPosition(glm::vec3 val);
		glm::vec3 scale()const;
		void setScale(glm::vec3 val); 
		glm::quat orientation()const;
		void setOrientation(glm::quat val); 


		Texture* texture() const; 
		void setTexture(Texture* val); 


		std::vector<glm::vec3>& controlPoints() ;
		void setControlPoints(std::vector<glm::vec3>& val);


		float height(); 
		void setHeight(float val);

		bool rayIntersectWall(glm::vec3& rayOrigin, glm::vec3& rayVector, glm::vec3& outIntersectionPoint, unsigned int& id);

		void drawTrianglesMesh(ShaderProgram& shader);

		int radius() const; 
		void setRadius(int val);


		float arcLength() const; 
		void setArcLength(float val); 
	private:

		

		void buildMesh();
		void buildPoints();

		std::vector<glm::vec3> myVertices;
		std::vector<glm::vec2> myTexcoords;
		std::vector<Triangle> myTriangles;

		Texture* myTexture;
		float myCylinderVertexCount = 32.f;
		int myRadius = 1;
		float myHeight;
		float myArcLength;

		unsigned int  myVAO;
		unsigned int myVBO;
		unsigned int myTboId;

		unsigned int myLineVAO;
		unsigned int myLineVBO;

		unsigned int myTrianglesWallVAO;
		unsigned int myTrianglesWallVBO;
		int myTrianglesWallSize;

		glm::vec3 myPosition;
		glm::vec3 myScale;
		glm::quat myOrientation;

		std::vector<glm::vec3> myControlPoints;

		

	};
}

#endif