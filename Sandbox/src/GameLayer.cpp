#include "GameLayer.h"
#include "iw/engine/Time.h"
#include "iw/engine/Components/Transform.h"
#include "iw/engine/Components/Model.h"
#include "iw/engine/Components/Camera.h"
#include "iw/renderer/Platform/OpenGL/GLDevice.h"
#include "iw/util/io/File.h"
#include "iw/input/Devices/Keyboard.h"
#include "iw/input/Devices/Mouse.h"

struct Player {
	float Speed;
	int DashFramesTotal;
	int DashFrames;
};

GameLayer::GameLayer() 
	: IwEngine::Layer("Game")
	, device(nullptr)
	, pipeline(nullptr)
{}

GameLayer::~GameLayer() {
	device->DestroyPipeline(pipeline);
	delete device;
}

int GameLayer::Initialize() {
	device = new IwRenderer::GLDevice();

	auto vs = device->CreateVertexShader  (iwu::ReadFile("res/sandboxvs.glsl").c_str());
	auto fs = device->CreateFragmentShader(iwu::ReadFile("res/sandboxfs.glsl").c_str());

	pipeline = device->CreatePipeline(vs, fs);
	device->SetPipeline(pipeline);

	IwEntity::Entity camera = space.CreateEntity();
	space.CreateComponent<IwEngine::Transform>(camera);
	float s = .05f;
	space.CreateComponent<IwEngine::Camera>(camera, iwm::matrix4::create_orthographic(1280 * s, 720 * s, 0, 1000));

	IwEntity::Entity player = space.CreateEntity();
	space.CreateComponent<IwEngine::Transform>(player, iwm::vector3(0, 0, 1));
	space.CreateComponent<IwEngine::Model>(player, loader.Load("res/quad.obj"), device);
	space.CreateComponent<Player>(player, 10.0f, 500);

	return 0;
}

void GameLayer::Update() {
	for (auto c : space.ViewComponents<IwEngine::Transform, Player>()) {
		auto& transform = c.GetComponent<IwEngine::Transform>();
		auto& player    = c.GetComponent<Player>();

		iwm::vector3 movement;
		if (IwInput::Keyboard::KeyDown(IwInput::LEFT)) {
			movement.x += 1;
		}

		if (IwInput::Keyboard::KeyDown(IwInput::RIGHT)) {
			movement.x -= 1;
		}

		if (IwInput::Keyboard::KeyDown(IwInput::UP)) {
			movement.y += 1;
		}

		if (IwInput::Keyboard::KeyDown(IwInput::DOWN)) {
			movement.y -= 1;
		}

		if (player.DashFrames > 0) {
			transform.Position += movement * player.Speed * player.DashFrames / 50 * IwEngine::Time::DeltaTime();
			player.DashFrames--;
		}

		else {
			if (IwInput::Keyboard::KeyDown(IwInput::X)) {
				player.DashFrames = player.DashFramesTotal;
			}

			else {
				transform.Position += movement * player.Speed * IwEngine::Time::DeltaTime();
			}
		}
	}

	for (auto c : space.ViewComponents<IwEngine::Transform, IwEngine::Camera>()) {
		auto& transform = c.GetComponent<IwEngine::Transform>();
		auto& camera    = c.GetComponent<IwEngine::Camera>();

		pipeline->GetParam("proj")
			->SetAsMat4(camera.Projection);

		pipeline->GetParam("view")
			->SetAsMat4(iwm::matrix4::create_look_at(
				transform.Position,
				transform.Position - transform.Forward(),
				transform.Up()));
	}

	for (auto c : space.ViewComponents<IwEngine::Transform, IwEngine::Model>()) {
		auto& transform = c.GetComponent<IwEngine::Transform>();
		auto& model     = c.GetComponent<IwEngine::Model>();

		transform.Rotation *= iwm::quaternion::create_from_euler_angles(0, 0, IwEngine::Time::DeltaTime());

		pipeline->GetParam("model")
			->SetAsMat4(transform.Transformation());

		for (int i = 0; i < model.MeshCount; i++) {
			device->SetVertexArray(model.Meshes[i].VertexArray);
			device->SetIndexBuffer(model.Meshes[i].IndexBuffer);
			device->DrawElements(model.Meshes[i].FaceCount, 0);
		}
	}
}
