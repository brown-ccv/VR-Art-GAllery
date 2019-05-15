#ifndef MODEL_H
#define MODEL_H
#include "VertexBuffer.h"
#include "Texture.h"


class ShaderProgram;
namespace GiGaPixelViewer
{
	class FBO;
}

class Model
{
public:
	
	~Model();

	VertexBuffer& objModel() const; 
	void setObjModel(VertexBuffer* val); 


	Texture& texture(int index) const; 
	void addTexture(Texture* val); 
	

	void render(ShaderProgram& shader);
	void render(ShaderProgram& shader, GiGaPixelViewer::FBO& fbo);

	glm::vec3& position();
	void setPosition(glm::vec3& val);

	glm::quat& orientation(); 
	void setOrientation(glm::quat& val); 

	glm::vec3& scale(); 
	void setScale(glm::vec3& val); 


private:

	Model();

	VertexBuffer* myObjModel;
	std::vector<Texture*> myTextures;

	friend class GLMLoader;
	friend class Terrain;

	glm::vec3 myPosition;
	glm::quat myOrientation;
	glm::vec3 myScale;
};

#endif