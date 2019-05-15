#include "WallMesh.h"
#include "common.h"
#include <cmath>
#include "ShaderProgram/ShaderProgram.h"
#include "Model/Texture.h"
#include "FBO.h"

namespace GiGaPixelViewer
{

	const float PI = 3.1416f;
	WallMesh::WallMesh()
	{
		buildMesh();
		buildPoints();
	}


	WallMesh::~WallMesh()
	{
		delete myTexture;
	}

	void WallMesh::buildMesh()
	{
		float i = 0.0f;
		float resolution = (PI / myCylinderVertexCount);
		myHeight =  myRadius * (PI/2);


		for (i = 0; i <= PI; i += resolution)
		{

			double x = cos(i);
			double z = sin(i);

			const float tc = (i / (float)(PI));
			glm::vec2 textP1(tc, 1.0);
			myTexcoords.push_back(textP1);

			glm::vec3 pointTop(myRadius*-x, myHeight, myRadius*-z);
			myVertices.push_back(pointTop);

			glm::vec2 textP2(tc, 0.0);
			myTexcoords.push_back(textP2);

			glm::vec3 pointBottom(myRadius*-x, 0, myRadius*-z);
			myVertices.push_back(pointBottom);


		}

		myArcLength = myRadius * PI;

		Triangle t1;
		Triangle t2;
		for (int i = 0; i < myVertices.size() - 2; i+=2)
		{

			t1.vertex0 = myVertices[i];
			t1.vertex1 = myVertices[i + 1];
			t1.vertex2 = myVertices[i + 2];

			myTriangles.push_back(t1);
			

			t2.vertex0 = myVertices[i + 2];
			t2.vertex1 = myVertices[i + 1];
			t2.vertex2 = myVertices[i + 3];			

			myTriangles.push_back(t2);

		}


		myControlPoints.push_back(glm::vec3(myRadius*-cos(0), 0.5f, myRadius*-sin(i)));
		myControlPoints.push_back(glm::vec3(myRadius*-cos(PI/2), 0.5f, myRadius*-sin(PI/2)));
		myControlPoints.push_back(glm::vec3(myRadius*-cos(PI), 0.5f, myRadius*-sin(PI)));

		/*glm::vec2 textP1(0.0, 0.0);
		myTexcoords.push_back(textP1);

		glm::vec3 point1(radius, 0, 0);
		vertices.push_back(point1);

		glm::vec2 textP2(1.0, 0.0);
		myTexcoords.push_back(textP2);

		glm::vec3 point2(radius, height, 0);
		vertices.push_back(point2);*/


		// Wall Mesh
		glGenVertexArrays(1, &myVAO);
		glBindVertexArray(myVAO);
		
		glGenBuffers(1, &myVBO);
		glBindBuffer(GL_ARRAY_BUFFER, myVBO);
		glBufferData(GL_ARRAY_BUFFER, myVertices.size() * sizeof(glm::vec3), &myVertices[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &myTboId);
		glBindBuffer(GL_ARRAY_BUFFER, myTboId);
		glBufferData(GL_ARRAY_BUFFER, myTexcoords.size() * sizeof(glm::vec2), &myTexcoords[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (GLvoid*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

		// Triangle Wall Mesh

		std::vector<glm::vec3> drawableTriangleVertices;
		for (int i= 0; i < myTriangles.size(); i++)
		{
			drawableTriangleVertices.push_back(myTriangles[i].vertex0);
			drawableTriangleVertices.push_back(myTriangles[i].vertex1);
			drawableTriangleVertices.push_back(myTriangles[i].vertex2);
		}
		
		/*drawableTriangleVertices.push_back(myTriangles[0].vertex0);
		drawableTriangleVertices.push_back(myTriangles[0].vertex1);
		drawableTriangleVertices.push_back(myTriangles[0].vertex2);
		drawableTriangleVertices.push_back(myTriangles[1].vertex0);
		drawableTriangleVertices.push_back(myTriangles[1].vertex1);
		drawableTriangleVertices.push_back(myTriangles[1].vertex2);*/

		glGenVertexArrays(1, &myTrianglesWallVAO);
		glBindVertexArray(myTrianglesWallVAO);

		glGenBuffers(1, &myTrianglesWallVBO);
		glBindBuffer(GL_ARRAY_BUFFER, myTrianglesWallVBO);
		glBufferData(GL_ARRAY_BUFFER, drawableTriangleVertices.size() * sizeof(glm::vec3), &drawableTriangleVertices[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

		myTrianglesWallSize = drawableTriangleVertices.size();

		myTexture = new Texture(GL_TEXTURE_2D, "dragonballzsuper.jpg");
		


	}

	void WallMesh::buildPoints()
	{
		glGenVertexArrays(1, &myLineVAO);
		glGenBuffers(1, &myLineVBO);
	 	// Allocate space and upload the data from CPU to GPU
		glBindVertexArray(myLineVAO);
	 	glBindBuffer(GL_ARRAY_BUFFER, myLineVBO);
		glBufferData(GL_ARRAY_BUFFER, myControlPoints.size() * sizeof(glm::vec3), &myControlPoints[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void WallMesh::draw(ShaderProgram& shader)
	{
		
		glm::mat4 m_model = glm::mat4(1.0f);
		m_model *= glm::translate(myPosition);
		m_model *= glm::toMat4(glm::quat());
		m_model *= glm::scale(myScale);
		shader.setUniform("m", m_model);

		glBindVertexArray(myVAO);
		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, myTexture->GetTextureId());
		glDrawArrays(GL_TRIANGLE_STRIP, 0, myVertices.size() );
		glBindVertexArray(0);
		
	}

	
	void WallMesh::draw(ShaderProgram& shader, FBO& fbo)
	{
		glm::mat4 m_model = glm::mat4(1.0f);
		m_model *= glm::translate(myPosition);
		m_model *= glm::toMat4(glm::quat());
		m_model *= glm::scale(myScale);
		shader.setUniform("m", m_model);

		glBindVertexArray(myVAO);
		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, fbo.texture());
		glDrawArrays(GL_TRIANGLE_STRIP, 0, myVertices.size());
		glBindVertexArray(0);
	}

	void WallMesh::drawControlPoints(ShaderProgram& shader)
	{

		glm::mat4 m_model = glm::mat4(1.0f);
		m_model *= glm::translate(myPosition);
		m_model *= glm::toMat4(glm::quat());
		m_model *= glm::scale(myScale);
		shader.setUniform("m", m_model);
		
		glBindVertexArray(myLineVAO);
		//glDisable(GL_DEPTH_TEST);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glDrawArrays(GL_POINTS, 0, myControlPoints.size());
		glDisable(GL_PROGRAM_POINT_SIZE);
		//glEnable(GL_DEPTH_TEST);
		glBindVertexArray(0);
	}

	bool WallMesh::rayIntersectWall(glm::vec3& rayOrigin, glm::vec3& rayVector, glm::vec3& outIntersectionPoint, unsigned int& id)
	{
		/* taken from https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm */
		
		for (int i = 0; i < myTriangles.size(); i++)
		{
			Triangle triangle = myTriangles[i];
			
			const float EPSILON = 0.0000001f;
			glm::vec3 vertex0 = triangle.vertex0 + myPosition;
			glm::vec3 vertex1 = triangle.vertex1 + myPosition;
			glm::vec3 vertex2 = triangle.vertex2 + myPosition;
			glm::vec3 edge1, edge2, h, s, q;
			float a, f, u, v;
			edge1 = vertex1 - vertex0;
			edge2 = vertex2 - vertex0;
			h = glm::cross(rayVector, edge2);
			a = glm::dot(edge1,h);
			if (a > -EPSILON && a < EPSILON)
			{
				continue;;    // This ray is parallel to this triangle.
			}
			f = 1.0f / a;
			s = rayOrigin - vertex0;
			u = f * (glm::dot(s,h));
			if (u < 0.0f || u > 1.0f)
			{
				continue;
			}
			q = glm::cross(s,edge1);
			v = f * glm::dot(rayVector,q);
			if (v < 0.0f || u + v > 1.0f)
			{
				continue;
			}
			// At this stage we can compute t to find out where the intersection point is on the line.
			float t = f * glm::dot(edge2,q);
			if (t > EPSILON) // ray intersection
			{
				outIntersectionPoint = rayOrigin + rayVector * t;
				id = i;
				return true;
			}
		}

		return false;

	}


	void WallMesh::drawTrianglesMesh(ShaderProgram& shader)
	{
		glm::mat4 m_model = glm::mat4(1.0f);
		m_model *= glm::translate(myPosition);
		m_model *= glm::toMat4(glm::quat());
		m_model *= glm::scale(myScale);
		shader.setUniform("m", m_model);

		glBindVertexArray(myTrianglesWallVAO);
		//glDisable(GL_DEPTH_TEST);
		glDrawArrays(GL_TRIANGLES, 0, myTrianglesWallSize);
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			std::cout << "glGetError: " << err << std::endl;
		}
		//glEnable(GL_DEPTH_TEST);
		glBindVertexArray(0);
	}

	glm::vec3 WallMesh::position() const
	{ 
		return myPosition; 
	}
	
	void WallMesh::setPosition(glm::vec3 val) 
	{ 
		myPosition = val; 
	}

	glm::vec3 WallMesh::scale() const 
	{
		return myScale; 
	}
	
	void WallMesh::setScale(glm::vec3 val) 
	{
		myScale = val; 
	}

	glm::quat WallMesh::orientation() const 
	{ 
		return myOrientation; 
	}

	void WallMesh::setOrientation(glm::quat val) 
	{ 
		myOrientation = val; 
	}

	Texture* WallMesh::texture() const
	{
		return myTexture; 
	}

	void WallMesh::setTexture(Texture* val)
	{
		myTexture = val; 
	}

	std::vector<glm::vec3>& WallMesh::controlPoints() 
	{
		return myControlPoints; 
	}

	void WallMesh::setControlPoints(std::vector<glm::vec3>& val)
	{
		myControlPoints = val; 
	}

	float WallMesh::height()
	{ 
		return myHeight; 
	}
	
	void WallMesh::setHeight(float val)
	{ 
		myHeight = val; 
	}

	int WallMesh::radius() const 
	{
		return myRadius; 
	}

	void WallMesh::setRadius(int val) 
	{ 
		myRadius = val; 
	}

	float WallMesh::arcLength() const
	{ 
		return myArcLength; 
	}
	
	void WallMesh::setArcLength(float val)
	{
		myArcLength = val; 
	}

}