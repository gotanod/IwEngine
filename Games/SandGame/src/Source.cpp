#include "iw/engine/EntryPoint.h"
#include <string>

#include "iw/engine/Events/Events.h"
#include "iw/engine/Layers/ImGuiLayer.h"
#include "iw/engine/Layers/ToolLayer.h"

using GETAPP_FUNC = iw::Application*(*)();

void Reload(std::wstring dll, HINSTANCE& hInst) {
	if (hInst) {
		FreeLibrary(hInst);
	}

	hInst = LoadLibrary(dll.c_str());

	if (!hInst) {
		LOG_ERROR << "Failed to load library " << dll.c_str();
	}
}

template<typename _t>
_t LoadFunction(HINSTANCE hInst, std::string function) {
	_t funcPtr = (_t)GetProcAddress(hInst, function.c_str());

	if (!funcPtr) {
		LOG_ERROR << "Failed to load function " << function;
	}

	return funcPtr;
}

class EditorApp : public iw::Application {
private:
	iw::InitOptions& options;

	iw::ImGuiLayer* imgui = nullptr;
	iw::Layer* toolbox = nullptr;
	iw::Layer* sandbox = nullptr;

public:
	iw::Application* m_gameNew = nullptr;
	iw::Application* m_game    = nullptr;

	EditorApp(iw::InitOptions& options): options(options) {}

	int Initialize(iw::InitOptions& options) {
		return InitGame();
	}

	void Run() {
		while (m_game) {
			if (!m_game->Window()->Handle()) {
				InitGame();
			}

			m_game->Run();
			
			delete m_game;
			m_game = m_gameNew; m_gameNew = nullptr;
		}
	}

	void ReloadGame() {
		HINSTANCE gameInst;
		Reload(L"a_wEditor.dll", gameInst);
		m_gameNew = LoadFunction<GETAPP_FUNC>(gameInst, "GetApplicationForEditor")();
	}

	int InitGame() {
		sandbox = m_game->GetLayer("Sandbox");
		
		if (imgui)         m_game->PushLayer(imgui);
		else       imgui = m_game->PushLayer<iw::ImGuiLayer>();

		if (toolbox)           m_game->PushLayer(toolbox);
		else         toolbox = m_game->PushLayer<iw::ToolLayer>(sandbox->GetMainScene());

		int err = m_game->Initialize(options);

		if (imgui) {
			imgui->BindContext();
		}

		m_game->Console->AddHandler([&](const iw::Command& command) {
			if (command.Verb == "reload") {
				m_game->PopLayer(imgui);
				m_game->PopLayer(toolbox);
				ReloadGame();
			}

			else if (command.Verb == "toolbox") { // in editor
				bool dev = GetLayer("Toolbox") != nullptr;
				if (dev) {
					m_game->PopLayer(toolbox);
					m_game->PushLayer(sandbox);
				}

				else {
					m_game->PushLayer(toolbox);
					m_game->PopLayer(sandbox);
				}
			}

			else if (command.Verb == "imgui") {
				if (GetLayer("ImGui") == nullptr) {
					m_game->PushLayer(imgui);
					Bus->push<iw::WindowResizedEvent>(Window()->Id(), Renderer->Width(), Renderer->Height());
				}

				else {
					m_game->PopLayer(imgui);
				}
			}

			return false;
		});

		return err;
	}
};

iw::Application* CreateApplication(
	iw::InitOptions& options)
{
	EditorApp* app = new EditorApp(options);
	app->ReloadGame();

	app->m_game = app->m_gameNew;

	return app;
}

