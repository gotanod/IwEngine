#pragma once

#include "Core.h"
#include "Window.h"

namespace IwEngine {
	class IWENGINE_API Application {
	private:
		Window* m_window;
		bool m_running;
	protected:
		Window& window;

	public:
		Application();
		virtual ~Application();

		virtual int Initilize();
		virtual void Start();
		virtual void Stop();
		virtual void Destroy();
		virtual void OnEvent(Event& e);
	private:
		void Run();
	};

	Application* CreateApplication();
}