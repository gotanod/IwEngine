#include "iw/engine/Application.h"
#include "iw/log/logger.h"
#include "iw/log/sink/async_std_sink.h"
#include "iw/log/sink/file_sink.h"
#include "iw/events/functional/callback.h"
#include "iw/engine/Time.h"
#include <atomic>

namespace IwEngine {
	Application::Application()
		: m_running(false)
		, m_window(IWindow::Create())
		, m_device(IwRenderer::IDevice::Create())
		, RenderQueue(*m_device)
	{
		m_imguiLayer = PushLayer<ImGuiLayer>();
	}

	Application::~Application() {
		delete m_window;
	}

	int Application::Initialize(
		InitOptions& options)
	{
		Time::Update();

		LOG_SINK(iwlog::async_stdout_sink, iwlog::INFO);
		LOG_SINK(iwlog::async_stderr_sink, iwlog::ERR);
		LOG_SINK(iwlog::file_sink,         iwlog::DEBUG, "sandbox.log");

		m_window->SetCallback(
			iwevents::make_callback(&Application::HandleEvent, this));

		int status;
		LOG_DEBUG << "Initializing window...";
		if (status = m_window->Initialize(options.WindowOptions)) {
			LOG_ERROR << "Window failed to initialize with error code "
				<< status;
			return status;
		}

		LOG_DEBUG << "Binding input manager to window...";

		m_window->SetInputManager(InputManager);

		for (Layer* layer : m_layers) {
			LOG_DEBUG << "Initializing " << layer->Name() << " layer...";
			if (status = layer->Initialize(options)) {
				LOG_ERROR << layer->Name()
					<< " layer initialization failed with error code "
					<< status;
				return status;
			}
		}

		//Need to set after so window doesn't send events before imgui gets initialized 
		m_window->SetState(options.WindowOptions.State);

		Time::Update();
		Time::SetFixedTime(1 / 60.0f);

		return 0;
	}

	void Application::Run() {
		m_running = true;

		std::atomic<int> threads = 0;
		float accumulatedTime = 0;

		m_updateThread = std::thread([&] {
			while (true) {
				LOG_INFO << m_workQueue.empty();
				while (!m_workQueue.empty()) {
					LOG_INFO << m_workQueue.pop()->Type;
					//DispatchEvent(event);
				}

				for (Layer* layer : m_layers) {
					layer->UpdateSystems();
				}

				threads = 0;
			}
		});

		while (m_running) {
			Time::Update();

			m_window->Clear();

			// Get SystemChains from layers
			
			// Clear work queue
			m_workQueue.clear();

			// PreUpdate
			LOG_DEBUG << "Pre-Update";
			for (Layer* layer : m_layers) {
				layer->PreUpdate();
			}

			// Kick off threads with copy of work queue
			threads = 1;
			LOG_DEBUG << "Post start";
			m_workQueue.push(new WindowResizedEvent(100, 100));

			while (threads > 0) {
			}

			//m_updateThread.join();

			// Update Layers
			LOG_DEBUG << "Update";
			for (Layer* layer : m_layers) {
				layer->Update();
			}

			// Break until work threads are done?
			LOG_DEBUG << "Post-Update";
			for (Layer* layer : m_layers) {
				layer->PostUpdate();
			}

			// ImGui render
			m_imguiLayer->Begin();
			for (Layer* layer : m_layers) {
				layer->ImGui();
			}
			m_imguiLayer->End();
			
			// Execute render queue
			m_window->Update();
			m_window->Render();

			//accumulatedTime += Time::DeltaTime();
			//while (accumulatedTime >= Time::FixedTime()) {
			//	FixedUpdate();
			//	accumulatedTime -= Time::FixedTime();
			//}
		}
	}

	void Application::Update() {

	}

	void Application::FixedUpdate() {
		for (Layer* layer : m_layers) {
			layer->FixedUpdate();
		}
	}

	void Application::Destroy() {
		m_running = false;
		m_window->Destroy();
		for (Layer* layer : m_layers) {
			layer->Destroy();
		}
	}

	void Application::HandleEvent(
		Event& e)
	{
		switch (e.Type) {
			case MouseWheel:    DispatchEvent((MouseWheelEvent&)e);    break;
			case MouseMoved:    DispatchEvent((MouseMovedEvent&)e);    break;
			case MouseButton:   DispatchEvent((MouseButtonEvent&)e);   break;
			case Key:    	    DispatchEvent((KeyEvent&)e);           break;
			case KeyTyped:      DispatchEvent((KeyTypedEvent&)e);      break;
			case WindowResized: DispatchEvent((WindowResizedEvent&)e); break;
			case WindowMoved:   /*dispatch event*/   break;
			case WindowClosed:  Destroy(); break;
			default: LOG_WARNING << "Application mishandled event " + e.Type; break;
		}
	}
}
