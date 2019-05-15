#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  

#include "GiGaPixelViewerVRApp.h"
#include "Model/GLMLoader.h"
#include "ScreenTileMap.h"
#include "FBO.h"
#include "Camera/Camera.h"

#define DEBUG_TILEMAP 1

namespace GiGaPixelViewer
{
	GiGaPixelViewerVRApp::GiGaPixelViewerVRApp(int argc, char *argv[]) : m_model(1.0f),
		m_vbo(0),
		m_vao(0),
		m_vs(0),
		m_fs(0),
		m_shader(0),
		myShowRay(false),
		myFboWidth(2048),
		myFboHeight(1024),
		myDebugTexture(false),
		myModelInitialized(false),
		myDebugTriangles(false),
		myCtrKeyPressed(false),
		VRApp(argc, argv)
	{

		if (argc > 3)
		{
			for (int index = 1; index < argc; ++index)
			{
				
				if (strcmp(argv[index], "-c") == 0)
				{
					std::string fileConf = argv[index + 1];
					if (fileConf.find("HTC_Vive.minvr") != string::npos)
					{
						myMode = vr;
					}
					else
					{
						myMode = desktop;
					}
				}

				if (strcmp(argv[index], "-f") == 0)
				{
					if (argv[index + 1])
					{
						myFilePath = argv[index + 1];
					}
					break;
				}
				
			}


		}

	}


	GiGaPixelViewerVRApp::~GiGaPixelViewerVRApp()
	{
		_CrtDumpMemoryLeaks();
		delete camera;
		delete objModel;
		delete myScreenTileMap;
		delete myWallMesh;
	}

	void GiGaPixelViewerVRApp::onButtonDown(const MinVR::VRButtonEvent &event)
	{
		if (!myModelInitialized)
		{
			return;
		}

		std::cout << event.getName() << std::endl;

		if (event.getName() == "KbdEsc_Down") {
			shutdown();
		}

		if (event.getName() == "KbdRight_Down") {

			myScreenTileMap->moveTiles(-1);
		}

		if (event.getName() == "KbdLeft_Down") {
			myScreenTileMap->moveTiles(1);
		}

		// zoom in
		if (event.getName() == "KbdUp_Down") 
		{
		

			//delete myFbo;
			//myFbo = new FBO();
			//myFboWidth /= 2;
			//myFboHeight /= 2;
			//myFbo->CreateFBO(myFboWidth, myFboHeight);
			myScreenTileMap->zoomIn(0.1f);
			
		}

		// zoom out
		if (event.getName() == "KbdDown_Down")
		{
			myScreenTileMap->zoomOut(0.1f);
		}


		if ( myMode != vr && myDebugTexture && event.getName() == "MouseBtnLeft_Down")
		{
			// mouse click on clip space
			glm::vec2 posMouseClipSpace(((2.0f * myCursorPos.x) / myDeviceWidth) - 1.0f,
				1.0f - ((2.0f *  myCursorPos.y) / myDeviceHeight));

			//std::cout << "pos mouse: " << posMouseClipSpace.x << " " << posMouseClipSpace.y << std::endl;
			
			myScreenTileMap->zoomIn(posMouseClipSpace);

		}

		if (myMode != vr && !myDebugTexture && event.getName() == "MouseBtnLeft_Down" && !myCtrKeyPressed)
		{
			glm::vec3 ray = rayCast(myCursorPos.x, myCursorPos.y);
			
			glm::vec3 intersectedPoint;
			unsigned int id;
			if (myWallMesh->rayIntersectWall(camera->position(), ray, intersectedPoint, id))
			{
				std::cout << "Hit wall: " << intersectedPoint.x << " " << intersectedPoint.y << " " << intersectedPoint.z << std::endl;

				
				/*vec2.x = atan2(intersectedPoint.z - myCylinder->position().z,
					myCylinder->position().x - intersectedPoint.x) / PI;*/

				glm::vec2 posRayClipSpace(2.f * atan2( intersectedPoint.x - myWallMesh->position().x
					, myWallMesh->position().z- intersectedPoint.z ) / PI,
					(2.f * (intersectedPoint.y - myWallMesh->position().y) ) / (myWallMesh->height()) - 1.f);

				std::cout << "Hit wall Clip Space: " << posRayClipSpace.x << " " << posRayClipSpace.y << std::endl;
				myScreenTileMap->zoomIn(posRayClipSpace);
				//myScreenTileMap->setMouseOnTileScreenSpace(posRayClipSpace);
			}
			
		}

		if (myMode != vr && !myDebugTexture && event.getName() == "MouseBtnLeft_Down" && myCtrKeyPressed)
		{
			glm::vec3 ray = rayCast(myCursorPos.x, myCursorPos.y);

			glm::vec3 intersectedPoint;
			unsigned int id;
			if (myWallMesh->rayIntersectWall(camera->position(), ray, intersectedPoint, id))
			{
				std::cout << "Hit wall: " << intersectedPoint.x << " " << intersectedPoint.y << " " << intersectedPoint.z << std::endl;


				/*vec2.x = atan2(intersectedPoint.z - myCylinder->position().z,
				myCylinder->position().x - intersectedPoint.x) / PI;*/

				glm::vec2 posRayClipSpace(2.f * atan2(intersectedPoint.x - myWallMesh->position().x
					, myWallMesh->position().z - intersectedPoint.z) / PI,
					(2.f * (intersectedPoint.y - myWallMesh->position().y)) / (myWallMesh->height()) - 1.f);

				std::cout << "Hit wall Clip Space: " << posRayClipSpace.x << " " << posRayClipSpace.y << std::endl;
				myScreenTileMap->zoomOut(posRayClipSpace);
				//myScreenTileMap->setMouseOnTileScreenSpace(posRayClipSpace);
			}
		}

		if (myMode == vr && !myDebugTexture 
			&& (event.getName() == "HTC_Controller_Right_GripButton_Down" || event.getName() == "HTC_Controller_2_GripButton_Down")  )
		{

			glm::vec3 from(myCamPos[0], myCamPos[1], myCamPos[2]);
			glm::vec3 to = from + (-20.0f * myRay);
			//glm::vec3 ray = rayCast(myCursorPos.x, myCursorPos.y);

			glm::vec3 intersectedPoint;
			unsigned int id;
			if (myWallMesh->rayIntersectWall(from, -myRay, intersectedPoint, id))
			{
				std::cout << "Hit wall: " << intersectedPoint.x << " " << intersectedPoint.y << " " << intersectedPoint.z << std::endl;


				/*vec2.x = atan2(intersectedPoint.z - myCylinder->position().z,
				myCylinder->position().x - intersectedPoint.x) / PI;*/

				/*glm::vec2 posRayClipSpace(2.f * atan2(intersectedPoint.x - myWallMesh->position().x
					, myWallMesh->position().z - intersectedPoint.z) / PI,
					(2.f * (intersectedPoint.y - myWallMesh->position().y)) / (myWallMesh->height()) - 1.f);*/

				glm::vec2 posRayClipSpace(2.f * atan2(intersectedPoint.x - myWallMesh->position().x
					, myWallMesh->position().z - intersectedPoint.z) / PI,
					(2.f * (intersectedPoint.y - myWallMesh->position().y)) / (myWallMesh->height()) - 1.f);

				std::cout << "Hit wall VR Clip Space: " << posRayClipSpace.x << " " << posRayClipSpace.y << std::endl;
				myScreenTileMap->zoomIn(posRayClipSpace);
				//myScreenTileMap->setMouseOnTileScreenSpace(posRayClipSpace);
			}
		}

		if (myMode == vr && !myDebugTexture
			&& (event.getName() == "HTC_Controller_Right_Axis1Button_Down" || event.getName() == "HTC_Controller_2_Axis1Button_Down"))
		{

			glm::vec3 from(myCamPos[0], myCamPos[1], myCamPos[2]);
			glm::vec3 to = from + (-20.0f * myRay);
			//glm::vec3 ray = rayCast(myCursorPos.x, myCursorPos.y);

			glm::vec3 intersectedPoint;
			unsigned int id;
			if (myWallMesh->rayIntersectWall(from, -myRay, intersectedPoint, id))
			{
				std::cout << "Hit wall: " << intersectedPoint.x << " " << intersectedPoint.y << " " << intersectedPoint.z << std::endl;


				/*vec2.x = atan2(intersectedPoint.z - myCylinder->position().z,
				myCylinder->position().x - intersectedPoint.x) / PI;*/

				/*glm::vec2 posRayClipSpace(2.f * atan2(intersectedPoint.x - myWallMesh->position().x
				, myWallMesh->position().z - intersectedPoint.z) / PI,
				(2.f * (intersectedPoint.y - myWallMesh->position().y)) / (myWallMesh->height()) - 1.f);*/

				glm::vec2 posRayClipSpace(2.f * atan2(intersectedPoint.x - myWallMesh->position().x
					, myWallMesh->position().z - intersectedPoint.z) / PI,
					(2.f * (intersectedPoint.y - myWallMesh->position().y)) / (myWallMesh->height()) - 1.f);

				std::cout << "Hit wall VR Clip Space: " << posRayClipSpace.x << " " << posRayClipSpace.y << std::endl;
				myScreenTileMap->zoomIn(posRayClipSpace);
				//myScreenTileMap->setMouseOnTileScreenSpace(posRayClipSpace);
			}
		}
		

		if (myMode != vr && !myDebugTexture && event.getName() == "MouseBtnRight_Down")
		{
			if (camera)
			{
				camera->SetCaRotate(true);
			}
		}

		if (myMode != vr && myDebugTexture && event.getName() == "MouseBtnRight_Down")
		{
			// mouse click on clip space
			glm::vec2 posMouseClipSpace((2.0f * myCursorPos.x) / myDeviceWidth - 1.0f,
				1.0f - (2.0f *  myCursorPos.y) / myDeviceHeight);

			//std::cout << "pos mouse: " << posMouseClipSpace.x << " " << posMouseClipSpace.y << std::endl;
			myScreenTileMap->zoomOut(posMouseClipSpace);
		}

		if (event.getName() == "KbdW_Down")
		{
			if (camera)
			{
				camera->Move(Camera::forward);
			}

		}

		if (event.getName() == "KbdS_Down")
		{
			if (camera)
			{
				camera->Move(Camera::backward);
			}
		}

		if (event.getName() == "KbdA_Down")
		{
			if (camera)
			{
				camera->Move(Camera::left);
			}
		}

		if (event.getName() == "KbdD_Down")
		{
			if (camera)
			{
				camera->Move(Camera::right);
			}
		}

		if (event.getName() == "KbdT_Down")
		{
			myDebugTexture = !myDebugTexture;
		}

		if (event.getName() == "KbdT_Down")
		{
			myDebugTexture = !myDebugTexture;
		}
		
		if (event.getName() == "KbdLeftControl_Down")
		{
			myCtrKeyPressed = true;
		}

		
		// Press escape to quit.
		if (event.getName() == "KbdEsc_Down") {
			shutdown();
		}
	}

	void GiGaPixelViewerVRApp::onButtonUp(const MinVR::VRButtonEvent &event)
	{
		if (!myModelInitialized)
		{
			return;
		}

		if (event.getName() == "MouseBtnRight_Up")
		{
			if (camera)
			{
				camera->SetCaRotate(false);
			}
		}


		if (event.getName() == "KbdW_Up")
		{
			if (camera)
			{
				camera->Stop(Camera::forward);
			}
		}

		if (event.getName() == "KbdS_Up")
		{
			if (camera)
			{
				camera->Stop(Camera::backward);
			}

		}

		if (event.getName() == "KbdA_Up")
		{
			if (camera)
			{
				camera->Stop(Camera::left);
			}
		}

		if (event.getName() == "KbdD_Up")
		{
			if (camera)
			{
				camera->Stop(Camera::right);
			}
		}
		
		if (event.getName() == "KbdT_Up")
		{
			myDebugTriangles = !myDebugTriangles;
		}
		
		if (event.getName() == "KbdLeftControl_Up")
		{
			myCtrKeyPressed = false;
		}

	}

	void GiGaPixelViewerVRApp::onCursorMove(const MinVR::VRCursorEvent &event)
	{

		if (!myModelInitialized)
		{
			return;
		}

		if (event.getName() == "Mouse_Move" && myMode != vr )
		{
			//std::cout << event.getName() << std::endl;
			const float* pos = event.getPos();
			myCursorPos = glm::vec2(pos[0], pos[1]);

			if (camera)
			{
				camera->RotateView(myCursorPos);
			}

		
			if (myDebugTexture)
			{
				glm::vec2 posMouseClipSpace((2.0f * myCursorPos.x) / myDeviceWidth - 1.0f,
					1.0f - (2.0f *  myCursorPos.y) / myDeviceHeight);

				std::cout << "pos mouse: " << posMouseClipSpace.x << " " << posMouseClipSpace.y << std::endl;

				myScreenTileMap->setMouseOnTileScreenSpace(posMouseClipSpace);
			}

		

		}
	}

	void GiGaPixelViewerVRApp::onTrackerMove(const MinVR::VRTrackerEvent &event)
	{

		if (!myModelInitialized)
		{
			return;
		}

		if (event.getName() == "HTC_Controller_2_Move" ||
			event.getName() == "HTC_Controller_Right_Move")
		{
			const float * transformM = event.getTransform();
			glm::quat q;
			q.w = sqrt(fmax(0, 1 + transformM[0] + transformM[5] + transformM[10])) / 2;
			q.x = sqrt(fmax(0, 1 + transformM[0] - transformM[5] - transformM[10])) / 2;
			q.y = sqrt(fmax(0, 1 - transformM[0] + transformM[5] - transformM[10])) / 2;
			q.z = sqrt(fmax(0, 1 - transformM[0] - transformM[5] + transformM[10])) / 2;
			q.x = copysign(q.x, transformM[9] - transformM[6]);
			q.y = copysign(q.y, transformM[2] - transformM[8]);
			q.z = copysign(q.z, transformM[4] - transformM[1]);

			myControllerOrientation = q;
			const float * ctrlPos = event.getPos();
			myControlerPosition = glm::vec3(ctrlPos[0], ctrlPos[1], ctrlPos[2]);

		}


		if (event.getName() == "HTC_HMD_1_Move"
			|| event.getName() == "HTC_HMD_Move")
		{
			const float * transformM = event.getTransform();
			myHDMposition = glm::vec3(transformM[12], transformM[13], transformM[14]);
			
			std::vector<glm::vec3> controlPoints = myWallMesh->controlPoints();
			float distance1 = glm::distance(controlPoints[0], myHDMposition);
			float distance2 = glm::distance(controlPoints[1], myHDMposition);
			float distance3 = glm::distance(controlPoints[2], myHDMposition);

			float minDist = std::min(distance1,distance2);
			minDist = std::min(minDist, distance3);

			//std::cout << "distance: " << minDist <<  std::endl;

			if (minDist <= 0.5f)
			{
				myScreenTileMap->setCurrentMipMap(4);
			}
			else if (minDist > 0.5f && minDist <= 1.0f)
			{
				myScreenTileMap->setCurrentMipMap(3);
			}
			else if (minDist > 1.0f && minDist <= 1.5f)
			{
				myScreenTileMap->setCurrentMipMap(2);
			}
			else if (minDist > 1.5f && minDist <= 4.0f)
			{
				myScreenTileMap->setCurrentMipMap(1);
			}
			else if(minDist > 4.0f)
			{
				myScreenTileMap->setCurrentMipMap(0);	
			}
			//std::cout << "currentMipMap: " << myScreenTileMap->currentMipMap() << std::endl;

		}

	}

	void GiGaPixelViewerVRApp::onAnalogChange(const MinVR::VRAnalogEvent &event)
	{
		/*if (event.getName().find("HTC_Controller_1_Trigger1") != -1
			|| event.getName().find("HTC_Controller_Right_Trigger1") != -1)
		{
			if (event.getValue() < 0.3)
			{
				myShowRay = false;

				return;
			}

			glm::quat q = myControllerOrientation;
			myRay.x = 2 * q.x*q.z - 2 * q.y*q.w;
			myRay.y = 2 * q.y*q.z + 2 * q.x*q.w;
			myRay.z = 1 - 2 * q.x * q.x - 2 * q.y * q.y;
			myRay = glm::normalize(myRay);


			myShowRay = true;
			//return;


		}*/

	}

	void GiGaPixelViewerVRApp::initGraphicsDrivers()
	{
		// Initialize GLEW.
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK) {
			std::cout << "Error initializing GLEW." << std::endl;
			return;
		}

		// Initialize OpenGL.
		glEnable(GL_DEPTH_TEST);
		glClearDepth(1.0f);
		glDepthFunc(GL_LEQUAL);
		glClearColor(0.0f, 0.3f, 0.2, 1.0f);

		

	}

	void GiGaPixelViewerVRApp::initShaders()
	{
		simpleShader.LoadShaders("Shaders/simpleShader.vert", "Shaders/simpleShader.frag");
		simpleShader.addUniform("m");
		simpleShader.addUniform("v");
		simpleShader.addUniform("p");
		simpleShader.addUniform("myTextureSampler");

		fsqShader.LoadShaders("Shaders/fsq.vert", "Shaders/fsq.frag");
		fsqShader.addUniform("screenTexture");


		lineShader.LoadShaders("Shaders/lineShader.vert", "Shaders/lineShader.frag");
		lineShader.addUniform("m");
		lineShader.addUniform("v");
		lineShader.addUniform("p");
		lineShader.addUniform("hitWall");

		pointShader.LoadShaders("Shaders/pointShader.vert", "Shaders/pointShader.frag");
		pointShader.addUniform("m");
		pointShader.addUniform("v");
		pointShader.addUniform("p");
		pointShader.addUniform("isPoint");
	}

	void GiGaPixelViewerVRApp::initModel()
	{
		// line vao/vbo
		glGenVertexArrays(1, &myRayVao);
		glGenBuffers(1, &myRayVbo);

		//load model
		std::string objFileName = "plane.obj";
		objModel = GLMLoader::loadModel(objFileName);
		m_texture = new Texture(GL_TEXTURE_2D, "dragonballzsuper.jpg");
		objModel->addTexture(m_texture);
		objModel->setPosition(glm::vec3(0.0f, 0.0f, -0.0f));
		//objModel->setScale(glm::vec3(2.0f, 1.0f, 1.0f));


		myWallMesh = new WallMesh();
		myWallMesh->setPosition(glm::vec3(0.0f, 0.5f, 0.0f));
		myWallMesh->setScale(glm::vec3(1.0f, 1.0f, 1.0f));

		//load tile image
#if DEBUG_TILEMAP
		if (!myFilePath.empty())
		{
			layer.createLayer(myFilePath);

		}
		else
		{
			std::cerr << "Usage: " << myFilePath << "-f PATH_TO_TIFF_FILE" << std::endl;
			assert(false);
		}
#endif



		float quadVertices[] = {
			// positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			1.0f, -1.0f,  1.0f, 0.0f,
			1.0f,  1.0f,  1.0f, 1.0f
		};

		glGenVertexArrays(1, &myFSQVAO);
		glGenBuffers(1, &myFSQVBO);
		glBindVertexArray(myFSQVAO);
		glBindBuffer(GL_ARRAY_BUFFER, myFSQVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	}

	void GiGaPixelViewerVRApp::renderTileMap(const MinVR::VRGraphicsState &renderState, int* viewport)
	{

		myScreenTileMap->Draw(viewport);




	}

	void GiGaPixelViewerVRApp::renderScene(const MinVR::VRGraphicsState &renderState)
	{
		if (objModel)
		{
			//glEnable(GL_CULL_FACE);
			//glCullFace(GL_FRONT_AND_BACK);
			
			if (!myDebugTriangles)
			{
				
				simpleShader.start();
				setProjectionViewMatrices(renderState, simpleShader);
				simpleShader.setUniformi("myTextureSampler",0);

				//objModel->render(simpleShader);
				myWallMesh->draw(simpleShader,*myFbo);

				simpleShader.stop();
				
			}
			else
			{
				pointShader.start();
				setProjectionViewMatrices(renderState, pointShader);
				pointShader.setUniformi("isPoint", 1);
				myWallMesh->drawControlPoints(pointShader);
				pointShader.stop();


				pointShader.start();
				setProjectionViewMatrices(renderState, pointShader);
				pointShader.setUniformi("isPoint", 0);
				myWallMesh->drawTrianglesMesh(pointShader);
				pointShader.stop();
			}
		


		}



	}

	void GiGaPixelViewerVRApp::renderRay(glm::vec3& from, glm::vec3& to)
	{

		std::vector<glm::vec3> rayVector;
		//rayVector.clear();
		rayVector.push_back(from);
		rayVector.push_back(to);

		glBindVertexArray(myRayVao);
		glBindBuffer(GL_ARRAY_BUFFER, myRayVbo);
		glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec3) * rayVector.size(),&rayVector[0],GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);

		glBindVertexArray(myRayVao);
		glDrawArrays(GL_LINES, 0, rayVector.size());
		//std::cout << "draw line " << std::endl;
		glBindVertexArray(0);
	}

	void GiGaPixelViewerVRApp::initCamera()
	{
		camera = new Camera(glm::vec3(0, 1.f, 0), glm::vec3(0, 0, -1));
	}

	void GiGaPixelViewerVRApp::initQuadBuffer(const MinVR::VRGraphicsState &renderState)
	{
		// Init quad Buffer
		const MinVR::VRDataIndex& index = renderState.index();
		// Get device height and width
		std::string strdevicewHeight = index.getValue("/WindowHeight");
		std::string strdeviceWidth = index.getValue("/WindowWidth");
	    myDeviceWidth = std::stoi(strdeviceWidth);
		myDeviceHeight = std::stoi(strdevicewHeight);
		myScreenTileMap = new ScreenTileMap(myDeviceWidth, myDeviceHeight, myFboWidth, myFboHeight ,layer);


		myFbo = new FBO();
		myFbo->CreateFBO(myFboWidth, myFboHeight);

	}

	void GiGaPixelViewerVRApp::setProjectionViewMatrices(const MinVR::VRGraphicsState &renderState, ShaderProgram& shader)
	{
		if (myMode == vr)
		{
			shader.setUniformMatrix4fv("p", renderState.getProjectionMatrix());
			shader.setUniformMatrix4fv("v", renderState.getViewMatrix());
		}
		else
		{
			const MinVR::VRDataIndex& index = renderState.index();
			// Get device height and width
			std::string strdevicewHeight = index.getValue("/WindowHeight");
			std::string strdeviceWidth = index.getValue("/WindowWidth");
			float deviceWidth = std::stof(strdeviceWidth);
			float devicewHeight = std::stof(strdevicewHeight);
			glm::mat4 projection = glm::perspective(glm::radians(45.0f), deviceWidth / devicewHeight, 0.1f, 100.0f);
			shader.setUniform("p", projection);
			shader.setUniform("v", camera->GetView());
		}
	}


	void GiGaPixelViewerVRApp::rayInterection(const MinVR::VRGraphicsState &renderState)
	{

	  

	}

	

	glm::vec3 GiGaPixelViewerVRApp::rayCast(float mouseX, float mouseY)
	{
		glm::vec4 rayClip((2.0f * mouseX) / myDeviceWidth - 1.0f,
			1.0f - (2.0f *  mouseY) / myDeviceHeight, -1.0, 1.0);
		glm::mat4 projection = glm::perspective(glm::radians(45.f), (float) (myDeviceWidth / myDeviceHeight), 0.1f, 1000.0f);
		glm::vec4 rayEye = inverse(projection) * rayClip;
		rayEye.z = -1.0f;
		rayEye.w = 0.0f;
		glm::mat4 view = camera->GetView();
		glm::mat4 invView = inverse(view);
		glm::vec4 temp = invView*rayEye;
		glm::vec3 rayWorld(temp.x, temp.y, temp.z);
		rayWorld = normalize(rayWorld);
		return rayWorld;
	}

	void GiGaPixelViewerVRApp::onRenderGraphicsContext(const MinVR::VRGraphicsState &renderState)
	{
		// Run setup if this is the initial call.
		if (renderState.isInitialRenderCall()) {

			initGraphicsDrivers();

			initCamera();
			
			initShaders();

			initQuadBuffer(renderState);

			initModel();

			
			myModelInitialized = true;
		



		}
	}

	void GiGaPixelViewerVRApp::onRenderGraphicsScene(const MinVR::VRGraphicsState &renderState)
	{

		// save MinVR state
		GLint currentFB;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFB);
		GLint m_viewport[4];
		glGetIntegerv(GL_VIEWPORT, m_viewport);

		// 1st pass
#if DEBUG_TILEMAP
		myFbo->Bind();
		//glViewport(0, 0, m_viewport[2]* 2, m_viewport[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		GLint m_viewport2[4] = {0,0,myFboWidth,myFboHeight };
		glViewport(0, 0, myFboWidth, myFboHeight/2);
		renderTileMap(renderState, m_viewport2);
		myFbo->Unbind();
#endif
	
		
		

		if (myDebugTexture)
		{
			glViewport(0, 0, myFboWidth, myFboHeight);
			fsqShader.setUniformi("screenTexture", 0);
			fsqShader.start();
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glBindVertexArray(myFSQVAO);
			glBindTexture(GL_TEXTURE_2D, myFbo->texture());	// use the color attachment texture as the texture of the quad plane
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
			fsqShader.stop();


			// return to saved state
			glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);
		}
		else
		{
			//std::cout << result << std::endl;

			glBindFramebuffer(GL_FRAMEBUFFER, currentFB);
			glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);

			// Clear the screen.
			glClearColor(0.0f, 0.3f, 0.2, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			renderScene(renderState);
			if (myMode == desktop)
			{
				camera->Update();
			}


			// Render Line
			if (myMode == vr)
			{

				glm::quat q = myControllerOrientation;
				myRay.x = 2 * q.x*q.z - 2 * q.y*q.w;
				myRay.y = 2 * q.y*q.z + 2 * q.x*q.w;
				myRay.z = 1 - 2 * q.x * q.x - 2 * q.y * q.y;
				myRay = glm::normalize(myRay);


				lineShader.start();
				myCamPos = renderState.getCameraPos();

				float lineDistance = -100.f;
				glm::vec3 from(myCamPos[0] , myCamPos[1] , myCamPos[2] );
				glm::vec3 to = from + (lineDistance * myRay);

				//Ray intersects
				
				glm::vec3 intersectedPoint;
				unsigned int id;
				bool hit = false;
				if (myWallMesh->rayIntersectWall(from, to, intersectedPoint, id))
				{
					float rayLength = glm::length( intersectedPoint - from);
					lineDistance = -rayLength;
					hit = true;
				}

				lineShader.setUniformi("hitWall", hit);

				glm::mat4 ModelMatrix;
				setProjectionViewMatrices(renderState, lineShader);
				ModelMatrix *= glm::translate(from);
				ModelMatrix *= glm::toMat4(glm::quat());
				ModelMatrix *= glm::scale(glm::vec3(1));
				lineShader.setUniform("m", ModelMatrix);
				to = from + (lineDistance * myRay);
				renderRay( from, to);
				lineShader.stop();

				
			}
			

		}


	}
}


