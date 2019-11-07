#pragma once

#include "EventBus.h"
#include "WindowOptions.h"
#include "iw/input/InputManager.h"

namespace IwEngine {
	class IWindow {
	public:
		virtual ~IWindow() {}

		virtual int Initialize(
			const WindowOptions& options) = 0;

		virtual void Destroy() = 0;
		virtual void Update()  = 0;
		//virtual void Clear() =   0;

		virtual void PollEvents()  = 0;
		virtual void SwapBuffers() = 0;

		virtual bool TakeOwnership()    = 0;
		virtual bool ReleaseOwnership() = 0;

		virtual void SetEventBus(
			EventBus& bus) = 0;

		virtual void SetInputManager(
			IwInput::InputManager& inputManager) = 0;

		virtual void SetState(
			DisplayState state) = 0;

		virtual void SetCursor(
			bool show) = 0;

		//virtual void SetDimensions(
		//	unsigned int width,
		//	unsigned int height) = 0;

		virtual unsigned int Id()     = 0;
		virtual unsigned int Width()  = 0;
		virtual unsigned int Height() = 0;
		virtual bool         Cursor() = 0;
		virtual DisplayState State()  = 0;

		static IWindow* Create();
	};

	class Window
		: public IWindow
	{
	protected:
		WindowOptions Options;
		IwInput::InputManager* InputManager;
		EventBus* Bus;

	public:
		virtual ~Window() {}

		inline unsigned int Id() {
			static unsigned int nextId = 0;
			static unsigned int id = nextId++;
			return id;
		}

		inline unsigned int Width() {
			return Options.Width;
		}

		inline unsigned int Height() {
			return Options.Height;
		}

		inline bool Cursor() {
			return Options.Cursor;
		}

		inline DisplayState State() {
			return Options.State;
		}
	};
}
