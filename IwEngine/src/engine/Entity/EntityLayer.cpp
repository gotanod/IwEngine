#include "iw/engine/Entity/EntityLayer.h"
#include "iw/renderer/Platform/OpenGL/GLDevice.h"
#include "iw/renderer/Platform/OpenGL/GLPipeline.h"
#include "iw/log/logger.h"
#include "imgui/imgui.h"
#include "iw/util/io/File.h"
#include "iw/engine/Time.h"

namespace IwEngine {
	EntityLayer::EntityLayer()
		: Layer("Entity")
		, viewTransform(iwm::matrix4::identity)
		, projTransform(iwm::matrix4::create_perspective_field_of_view(
			1.2f, 1.78f, 0.1f, 1000.0f))
		, lightColor(iwm::vector3::one)
		, lightAngle(0.0f)
		, specularScale(0.0f)
	{}

	EntityLayer::~EntityLayer() {}

	Model LoadModel(
		const char* name,
		IwGraphics::ModelLoader& loader,
		IwRenderer::IDevice* device) 
	{
		IwGraphics::ModelData* obj = loader.Load(name);

		Mesh* meshes = new Mesh[obj->MeshCount];

		for (size_t i = 0; i < obj->MeshCount; i++) {
			IwRenderer::IIndexBuffer* ib = device->CreateIndexBuffer(
				obj->Indices[i].FaceCount,
				obj->Indices[i].Faces);

			IwRenderer::VertexBufferLayout* vbl
				= new IwRenderer::VertexBufferLayout();
			vbl->Push<float>(3);
			vbl->Push<float>(3);

			IwRenderer::IVertexBuffer* vb = device->CreateVertexBuffer(
				obj->Meshes[i].VertexCount * sizeof(IwGraphics::Vertex),
				obj->Meshes[i].Vertices);

			IwRenderer::IVertexArray* va
				= device->CreateVertexArray(1, &vb, &vbl);

			meshes[i] = Mesh {va, ib, obj->Indices[i].FaceCount };
		}

		return { meshes, obj->MeshCount };
	}

	int EntityLayer::Initialize() {
		//Create rendering device
		device = new IwRenderer::GLDevice();

		//Create models
		Model lampModel = LoadModel("res/lamp.obj", loader, device);
		Model bearModel = LoadModel("res/bear.obj", loader, device);

		//Creating entity
		IwEntity::Entity lamp = space.CreateEntity();
		IwEntity::Entity bear = space.CreateEntity();

		Transform& lampTransform = space.CreateComponent<Transform>(lamp);
		lampTransform.Position.x += 20;

		Transform& bearTransform = space.CreateComponent<Transform>(bear);
		bearTransform.Position.x -= 20;

		space.CreateComponent<Velocity>(lamp);
		space.CreateComponent<Velocity>(bear);

		space.CreateComponent<Model>(lamp, lampModel);
		space.CreateComponent<Model>(bear, bearModel);

		//space.Sort();

		//Creating shader pipeline
		IwRenderer::IVertexShader* vs = device->CreateVertexShader(
			iwu::ReadFile("res/defaultvs.glsl").c_str());

		IwRenderer::IFragmentShader* fs = device->CreateFragmentShader(
			iwu::ReadFile("res/defaultfs.glsl").c_str());

		pipeline = device->CreatePipeline(vs, fs);

		return 0;
	}

	void EntityLayer::Destroy() {
		auto view = space.ViewComponents<Model>();
		for (auto entity : view) {
			Model& model = entity.GetComponent<Model>();
			for (int i = 0; i < model.MeshCount; i++) {
				device->DestroyVertexArray(model.Meshes[i].Vertices);
				device->DestroyIndexBuffer(model.Meshes[i].Indices);
			}
		}

		device->DestroyPipeline(pipeline);
	}

	void EntityLayer::Update() {
		lightAngle += Time::DeltaTime();

		float x = cos(lightAngle) * 100;
		float z = sin(lightAngle) * 100;

		auto view = space.ViewComponents<Transform, Velocity, Model>();
		for (auto entity : view) {
			Transform& transform = entity.GetComponent<Transform>();
			Velocity& velocity = entity.GetComponent<Velocity>();
			Model& model = entity.GetComponent<Model>();

			device->SetPipeline(pipeline);

			pipeline->GetParam("model")
				->SetAsMat4(transform.GetTransformation());

			pipeline->GetParam("view")
				->SetAsMat4(viewTransform);

			pipeline->GetParam("proj")
				->SetAsMat4(projTransform);

			pipeline->GetParam("lightPos")
				->SetAsVec3(iwm::vector3(x, 0, z));

			pipeline->GetParam("lightColor")
				->SetAsVec3(lightColor);

			pipeline->GetParam("specularScale")
				->SetAsFloat(specularScale);

			for (int i = 0; i < model.MeshCount; i++) {
				device->SetVertexArray(model.Meshes[i].Vertices);
				device->SetIndexBuffer(model.Meshes[i].Indices);
				device->DrawElements(model.Meshes[i].Count, 0);
			}

			transform.Position += velocity.Velocity;
		}
	}

	void EntityLayer::ImGui() {
		ImGui::Begin("Entity layer");

		auto view = space.ViewComponents<Transform>();
		for (auto entity : view) {
			Transform& transform = entity.GetComponent<Transform>();

			ImGui::Text("Pos (x, y, z): %f %f %f", 
				transform.Position.x, 
				transform.Position.y, 
				transform.Position.z);

			ImGui::Text("Rot (y): %f", 
				transform.Rotation.euler_angles().y);
		}

		ImGui::SliderFloat3("Light color", &lightColor.x, 0, 1);
		ImGui::SliderFloat("Specular scale", &specularScale, 0, 10);

		ImGui::End();
	}

	bool EntityLayer::On(
		WindowResizedEvent& event)
	{
		return false;
	}

	bool EntityLayer::On(
		MouseMovedEvent& event)
	{
		return false;
	}

	bool EntityLayer::On(
		MouseButtonEvent& event)
	{
		float speed = event.State ? Time::DeltaTime() * 15 : 0.0f;
		
		auto view = space.ViewComponents<Velocity>();
		for (auto entity : view) {
			Velocity& velocity = entity.GetComponent<Velocity>();

			if (event.Button == IwInput::MOUSE_L_BUTTON) {
				velocity.Velocity.z = -speed;
			}

			else if (event.Button == IwInput::MOUSE_R_BUTTON) {
				velocity.Velocity.z = speed;
			}

			else if (event.Button == IwInput::MOUSE_X1_BUTTON) {
				velocity.Velocity.x = speed;
			}

			else if (event.Button == IwInput::MOUSE_X2_BUTTON) {
				velocity.Velocity.x = -speed;
			}
		}

		return false;
	}

	bool EntityLayer::On(
		MouseWheelEvent& event)
	{
		auto view = space.ViewComponents<Transform>();
		for (auto entity : view) {
			Transform& transform = entity.GetComponent<Transform>();

			transform.Rotation *= iwm::quaternion::create_from_euler_angles(
				0, event.Delta * .1f, 0);
		}

		return false;
	}
}
