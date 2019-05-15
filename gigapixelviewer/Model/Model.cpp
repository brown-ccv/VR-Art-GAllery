#include "Model.h"
#include "GLMLoader.h"
#include <string>
#include <cassert>
#include "../FBO.h"
#include "../ShaderProgram/ShaderProgram.h"


Model::Model():myPosition(0),myOrientation(glm::quat()),myScale(1)
{

}


Model::~Model()
{
	delete myObjModel;
	for (int i = 0 ; i < myTextures.size(); ++i)
	{
		delete myTextures[i];
	}
		
}

VertexBuffer& Model::objModel() const
{
	return *myObjModel;
}

void Model::setObjModel(VertexBuffer* val)
{
	myObjModel = val;
}

Texture& Model::texture(int index) const
{
	return *myTextures[index];
}

void Model::addTexture(Texture* val)
{
	myTextures.push_back(val);
}

void Model::render(ShaderProgram& shaderProgram)
{

	glm::mat4 m_model = glm::mat4(1.0f);
	m_model *= glm::translate(myPosition);
	m_model *= glm::toMat4(myOrientation);
	m_model *= glm::scale(myScale);

	shaderProgram.setUniform("m", m_model);

	assert(myObjModel && "NO MODEL TO RENDER");
	// bind texture
	for (int i = 0; i < myTextures.size(); ++i)
	{
		myTextures[i]->Bind(i);
	}

	//render geometry
	if (myObjModel)
	{
		myObjModel->render();
	}
}

void Model::render(ShaderProgram& shaderProgram, GiGaPixelViewer::FBO& fbo)
{
	glm::mat4 m_model = glm::mat4(1.0f);
	m_model *= glm::translate(myPosition);
	m_model *= glm::toMat4(myOrientation);
	m_model *= glm::scale(myScale);

	shaderProgram.setUniform("m", m_model);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fbo.texture());

	//render geometry
	if (myObjModel)
	{
		myObjModel->render();
	}

	
}

glm::vec3& Model::position() 
{
	return myPosition;
}

void Model::setPosition(glm::vec3& val)
{
	myPosition = val;
}

glm::quat& Model::orientation() 
{
	return myOrientation;
}

void Model::setOrientation(glm::quat& val)
{
	myOrientation = val;
}

glm::vec3& Model::scale() 
{
	return myScale;
}

void Model::setScale(glm::vec3& val)
{
	myScale = val;
}
