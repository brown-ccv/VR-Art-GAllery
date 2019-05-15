#ifndef GIGAPIXELVIEWERVRAPP_H
#define GIGAPIXELVIEWERVRAPP_H

#include "common.h"
#include "Model/Model.h"
#include "ShaderProgram/ShaderProgram.h"

#include "Layer.h"
#include "WallMesh.h"

class Camera;
namespace GiGaPixelViewer
{
	class ScreenTileMap;
	class FBO;
	
	class GiGaPixelViewerVRApp : public MinVR::VRApp
	{

		enum Mode
		{
			vr,
			desktop
		};

	public:
		GiGaPixelViewerVRApp(int argc, char *argv[]);
		~GiGaPixelViewerVRApp();

		/**
		* Called whenever an new input event happens.
		*/
		virtual void onAnalogChange(const MinVR::VRAnalogEvent &state) override;

		virtual void onButtonDown(const MinVR::VRButtonEvent &state) override;

		virtual void onButtonUp(const MinVR::VRButtonEvent &state) override;

		virtual void onCursorMove(const MinVR::VRCursorEvent &state) override;

		virtual void onTrackerMove(const MinVR::VRTrackerEvent &state) override;

		/**
		* Called before renders to allow the user to set context-specific variables.
		*/
		void onRenderGraphicsContext(const MinVR::VRGraphicsState &renderState) override;

		/**
		* Called when the application draws.
		*/
		void onRenderGraphicsScene(const MinVR::VRGraphicsState &renderState) override;

	protected:
		void initGraphicsDrivers();

		void initShaders();

		void initModel();

		void renderTileMap(const MinVR::VRGraphicsState &renderState, int* viewport);
		
		void renderScene(const MinVR::VRGraphicsState &renderState);

		void renderRay(glm::vec3& from, glm::vec3& to);
	
		void initCamera();

		void initQuadBuffer(const MinVR::VRGraphicsState &renderState);

		void setProjectionViewMatrices(const MinVR::VRGraphicsState &renderState, ShaderProgram& shader);

		void rayInterection(const MinVR::VRGraphicsState &renderState);

		glm::vec3 rayCast(float mouseX, float mouseY);

	private:

		

		glm::mat4 m_model;

		GLuint m_vbo;
		GLuint m_vao;

		GLuint m_vs;
		GLuint m_fs;
		GLuint m_shader;

		Model* objModel;


		ShaderProgram simpleShader;
		ShaderProgram fsqShader;
		ShaderProgram lineShader;
		ShaderProgram pointShader;

		Camera* camera;
		Layer layer;

		glm::vec2 myCursorPos;

		FBO* myFbo;
		unsigned int myFboWidth;
		unsigned int myFboHeight;
		ScreenTileMap* myScreenTileMap;
		std::string myFilePath;
		int activeLayer;


		glm::vec3 distance;
		int myDeviceWidth; 
		int myDeviceHeight;


		unsigned int myFSQVAO;
		unsigned int myFSQVBO;
		Texture* m_texture;


		WallMesh* myWallMesh;
		Mode myMode;

		glm::vec3 myRay;
		GLuint myRayVao;
		GLuint myRayVbo;
		bool myShowRay;

		bool myDebugTriangles;

		glm::quat myControllerOrientation;
		glm::vec3 myControlerPosition;
		glm::vec3  myHDMposition;

		bool myDebugTexture;

		bool myModelInitialized;
		bool myCtrKeyPressed;
		const float* myCamPos;
	};


}


#endif