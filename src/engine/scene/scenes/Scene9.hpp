#include <glm/glm.hpp>
#include <functional>
#include "engine/scene/IScene.h"
#include "engine/graphics/Renderer.h"
#include "engine/managers/ShaderManager.h"
#include "engine/managers/ObjectManager.h"
#include "engine/managers/MaterialManager.h"
#include "engine/managers/InputManager.h"
#include "engine/scene/GameObject.h"
#include "engine/scene/Camera.h"
#include "engine/scene/Light.h"
#include "engine/utils/OBJReader.h"
#include "engine/components/MeshRenderer.h"
#include "engine/components/examples/RotateComponent.hpp"

namespace LittleEngine
{
	class Scene9 : public IScene
	{
	private:
		const char* OBJ_PATH = "../objs/utah_teapot.obj";
		const char* TEX01_PATH = "../textures/earth.png";

		const char* PROGRAM_NAME = "Program1";
		const char* VERT_SHADER_NAME = "VertexShaderDisplacement";
		const char* TESC_SHADER_NAME = "TescShaderDisplacement";
		const char* TESE_SHADER_NAME = "TeseShaderDisplacement";
		const char* GEOM_SHADER_NAME = "GeomShaderDisplacement";
		const char* FRAG_SHADER_NAME = "FragmentShaderDisplacement";
		const char* VERT_SHADER_PATH = "../shaders/adaptativeGeom.vert";
		const char* TESC_SHADER_PATH = "../shaders/adaptativeGeom.tesc";
		const char* TESE_SHADER_PATH = "../shaders/adaptativeGeom.tese";
		const char* GEOM_SHADER_PATH = "../shaders/adaptativeGeom.geom";
		const char* FRAG_SHADER_PATH = "../shaders/adaptativeGeom.frag";

		const char* PROGRAM_NAME_PP = "ProgramPP";
		const char* VERTEX_SHADER_NAME_PP = "VertexShaderPP";
		const char* FRAGMENT_SHADER_NAME_PP = "FragmentShaderPP";
		const char* VERTEX_SHADER_PATH_PP = "../shaders/postProcessing.vert";
		const char* FRAGMENT_SHADER_PATH_PP = "../shaders/postProcessing.frag";

		Renderer* renderer;
		std::vector<GameObject*> gameObjects;
		std::vector<Light*> lights;
		Camera* camera;

		void createPrograms()
		{
			//	TODO: Make object for programs config

			ShaderManager::instance()
				->createProgram(PROGRAM_NAME, RenderMode::TRIANGLES_PATCH)
				->createShader(VERT_SHADER_NAME, VERT_SHADER_PATH, GL_VERTEX_SHADER)
				->createShader(TESC_SHADER_NAME, TESC_SHADER_PATH, GL_TESS_CONTROL_SHADER)
				->createShader(TESE_SHADER_NAME, TESE_SHADER_PATH, GL_TESS_EVALUATION_SHADER)
				->createShader(GEOM_SHADER_NAME, GEOM_SHADER_PATH, GL_GEOMETRY_SHADER)
				->createShader(FRAG_SHADER_NAME, FRAG_SHADER_PATH, GL_FRAGMENT_SHADER)
				->assignShader(VERT_SHADER_NAME, PROGRAM_NAME)
				->assignShader(TESC_SHADER_NAME, PROGRAM_NAME)
				->assignShader(TESE_SHADER_NAME, PROGRAM_NAME)
				->assignShader(GEOM_SHADER_NAME, PROGRAM_NAME)
				->assignShader(FRAG_SHADER_NAME, PROGRAM_NAME)
				->loadProgram(PROGRAM_NAME)

				//	PostProcessing Program
				->createProgram(PROGRAM_NAME_PP)
				->createShader(VERTEX_SHADER_NAME_PP, VERTEX_SHADER_PATH_PP, GL_VERTEX_SHADER)
				->assignShader(VERTEX_SHADER_NAME_PP, PROGRAM_NAME_PP)
				->createShader(FRAGMENT_SHADER_NAME_PP, FRAGMENT_SHADER_PATH_PP, GL_FRAGMENT_SHADER)
				->assignShader(FRAGMENT_SHADER_NAME_PP, PROGRAM_NAME_PP)
				->loadProgram(PROGRAM_NAME_PP);
		}

		void createGameObjects()
		{
			//TODO: FUTURE IMPLEMENTATIONS - CREATE CONFIGURATION FILES CONTAINING OBJECTS.

			camera = new Camera(0, "CAMERA", CameraProjection::PERSPECTIVE);
			camera->transform->position.y = 2.f;
			camera->transform->position.z = 6.f;

			gameObjects.push_back(camera);

			GameObject* obj = new GameObject(1, "OBJ_TEST");
			MeshRenderer* meshRendererOBJ = new MeshRenderer();
			RotateComponent* rotateComponent = new RotateComponent();

			MaterialManager::instance()
				->createMaterial("MAT01")
				->loadTexture(
					"TEX01",
					TEX01_PATH,
					ShaderManager::instance()
					->getProgram(PROGRAM_NAME)
					->getVariableId("colorTex", LittleEngine::VariableType::UNIFORM))
				->asignTextureToMaterial("TEX01", "MAT01");

			meshRendererOBJ->setMesh(ObjectManager::instance()->loadMesh("Mesh", OBJ_PATH));
			meshRendererOBJ->initializeVAOData(ShaderManager::instance()->getProgram(PROGRAM_NAME));
			meshRendererOBJ->setMaterial(MaterialManager::instance()->getMaterial("MAT01"));

			obj->addComponent(meshRendererOBJ);
			obj->addComponent(rotateComponent);


			gameObjects.push_back(obj);

		}

		void uploadUniformsToProgram(ProgramObject* program)
		{
			renderer->setWireframeWidth(2.0);
			float zOffset = 0.03f;
			float jump = 7.0f;
			float maxDivs = 7.0f;
			glm::vec3 wireColor = glm::vec3(1.0, 1.0, 0.0);
			renderer->uploadUniformVariable(program, "zOffset", &zOffset);
			renderer->uploadUniformVariable(program, "wireColor", &wireColor);
			renderer->uploadUniformVariable(program, "jump", &jump);
			renderer->uploadUniformVariable(program, "maxDivs", &maxDivs);
		}

	public:

		~Scene9() {
			for (GameObject* gameObject : gameObjects)
			{
				delete gameObject;
			}
			gameObjects.clear();
			ObjectManager::instance()->destroy();
			MaterialManager::instance()->destroy();
			ShaderManager::instance()->destroy();
			delete renderer;
			delete[] this;
		}

		void resize(int width, int height)
		{
			renderer->resize(width, height);
			camera->updateProportions(width, height);
		}

		void load()
		{
			createPrograms();
			createGameObjects();
			renderer = new Renderer();
			renderer
				->startRenderConfig()
				->createFBO(ShaderManager::instance()->getProgram(PROGRAM_NAME_PP));

			std::function<void(const void*, const size_t)> f = [=](const void* data, const size_t classId) {
				this->inputHandle(data, classId);
			};

			InputManager::instance()->subscribe("SCENE09", f);
			for (GameObject* go : gameObjects)
			{
				go->onStart();
			}
		}

		void render()
		{
			renderer->clearBuffersFw();
			ShaderManager::instance()->useProgram(PROGRAM_NAME);
			uploadUniformsToProgram(ShaderManager::instance()->getProgram(PROGRAM_NAME));
			for (GameObject* go : gameObjects)
			{
				go->onRender(ShaderManager::instance()->getProgram(PROGRAM_NAME), camera->getViewProj());
			}
			renderer->clearBuffersPP();
			ShaderManager::instance()->useProgram(PROGRAM_NAME_PP);
			renderer->renderFBO(ShaderManager::instance()->getProgram(PROGRAM_NAME_PP));
		}

		void update(double deltaTime)
		{
			for (GameObject* go : gameObjects)
			{
				go->onUpdate(deltaTime);
			}
		}

		void inputHandle(const void* data, const size_t classId)
		{
			if (classId == typeid(KeyMessage).hash_code())
			{
				const KeyMessage* message = reinterpret_cast<const KeyMessage*>(data);
				int a = message->k;
				if (a == 333)
				{
					camera->transform->position.z = std::min(15.f, camera->transform->position.z += 0.5);
				}
				else if (a == 334)
				{
					camera->transform->position.z = std::max(2.0f, camera->transform->position.z -= 0.5);
				}
			}
		}
	};
}