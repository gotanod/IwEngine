#include "Platform/Windows/WindowsWindow.h"
#include "gl/glew.h"
#include "gl/wglew.h"
#include <cstdio>
#include <iostream>

ATOM RegClass(
	HINSTANCE instance,
	WNDPROC wndproc);

namespace IwEngine {
	Window* Window::Create() {
		return new WindowsWindow();
	}

	int WindowsWindow::Initilize() {
		AllocConsole();
		FILE* fp;
		freopen_s(&fp, "CONOUT$", "w", stdout);

		//LPTSTR window = 
		MAKEINTATOM(RegClass(m_instance, _WndProc));

		HWND fake_hwnd = CreateWindow(
			"Core", "Fake Window",
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			0, 0,
			1, 1,
			NULL, NULL,
			m_instance, NULL);

		HDC fake_dc = GetDC(fake_hwnd);

		PIXELFORMATDESCRIPTOR fake_pfd;
		ZeroMemory(&fake_pfd, sizeof(fake_pfd));
		fake_pfd.nSize = sizeof(fake_pfd);
		fake_pfd.nVersion = 1;
		fake_pfd.dwFlags = PFD_DRAW_TO_WINDOW
			| PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		fake_pfd.iPixelType = PFD_TYPE_RGBA;
		fake_pfd.cColorBits = 32;
		fake_pfd.cAlphaBits = 8;
		fake_pfd.cDepthBits = 24;

		INT fake_pfdId = ChoosePixelFormat(fake_dc, &fake_pfd);
		if (fake_pfdId == 0) {
			std::cout << "ChoosePixelFormat() failed!";
			return 1;
		}

		if (!SetPixelFormat(fake_dc, fake_pfdId, &fake_pfd)) {
			std::cout << "SetPixelFormat() failed!";
			return 1;
		}

		HGLRC fake_rc = wglCreateContext(fake_dc);
		if (fake_rc == 0) {
			std::cout << "wglCreateContext() failed!";
			return 1;
		}

		if (!wglMakeCurrent(fake_dc, fake_rc)) {
			std::cout << "wglMakeCurrent() failed!";
			return 1;
		}

		if (glewInit() != GLEW_OK) {
			std::cout << "glewInit() failed!";
			return 1;
		}

		m_window = CreateWindow(
			"Core", "Space",
			WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			100, 100,
			1280, 720,
			NULL, NULL,
			m_instance, NULL);

		SetWindowLongPtr(m_window, GWLP_USERDATA, (LONG_PTR)this);

		m_device = GetDC(m_window);

		CONST INT pixelAttribs[] = {
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
			WGL_COLOR_BITS_ARB, 32,
			WGL_ALPHA_BITS_ARB, 8,
			WGL_DEPTH_BITS_ARB, 24,
			WGL_STENCIL_BITS_ARB, 8,
			WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
			WGL_SAMPLES_ARB, 4,
			0
		};

		INT pfid;
		UINT formatCount;
		BOOL status = wglChoosePixelFormatARB(m_device, pixelAttribs,
			NULL, 1, &pfid, &formatCount);

		if (status == FALSE || formatCount == 0) {
			std::cout << "wglChoosePixelFormatARB() failed!";
			return 1;
		}

		PIXELFORMATDESCRIPTOR pfd;
		DescribePixelFormat(m_device, pfid, sizeof(pfd), &pfd);
		SetPixelFormat(m_device, pfid, &pfd);

		CONST INT major = 4, minor = 5;
		INT contextAttribs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, major,
			WGL_CONTEXT_MINOR_VERSION_ARB, minor,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0
		};

		m_context = wglCreateContextAttribsARB(m_device, 0, contextAttribs);
		if (m_context == NULL) {
			std::cout << "wglCreateContextAttribsARB() failed!";
			return 1;
		}

		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(fake_rc);
		ReleaseDC(fake_hwnd, fake_dc);
		DestroyWindow(fake_hwnd);
		if (!wglMakeCurrent(m_device, m_context)) {
			std::cout << "wglMakeCurrent() failed.";
			return 1;
		}
		
		DrawCursor(false);
		SetDisplayState(NORMAL);

		return 0;
	}

	void WindowsWindow::Destroy() {
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_context);
		ReleaseDC(m_window, m_device);
		DestroyWindow(m_window);
	}

	void WindowsWindow::Update() {
		MSG msg;
		while (PeekMessage(&msg, m_window, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void WindowsWindow::SetDisplayState(
		DisplayState state)
	{
		int wstate = -1;
		switch (state) {
		case HIDDEN:    wstate = SW_HIDE;       break;
		case MINIMIZED: wstate = SW_MINIMIZE;   break;
		case MAXIMIZED: wstate = SW_MAXIMIZE;   break;
		case NORMAL:    wstate = SW_SHOWNORMAL; break;
		}

		if (wstate != -1) {
			ShowWindow(m_window, wstate);
		}
	}

	void WindowsWindow::SetCallback(
		EventCallback callback)
	{
		m_callback = callback;
	}

	void WindowsWindow::DrawCursor(
		bool show)
	{
		ShowCursor(show);
	}

	LRESULT CALLBACK WindowsWindow::_WndProc(
		HWND hwnd,
		UINT msg,
		WPARAM wparam,
		LPARAM lparam)
	{
		WindowsWindow* me = reinterpret_cast<WindowsWindow*>(
			GetWindowLongPtr(hwnd, GWLP_USERDATA));
		if (me) {
			return me->WndProc(hwnd, msg, wparam, lparam);
		}

		return DefWindowProc(hwnd, msg, wparam, lparam);
	}

	LRESULT CALLBACK WindowsWindow::WndProc(
		HWND hwnd,
		UINT msg,
		WPARAM wparam,
		LPARAM lparam)
	{
		Event e = { false, msg };

		m_callback(e);

		if (!e.handled) {
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}

		return 0;
	}
}

ATOM RegClass(
	HINSTANCE instance,
	WNDPROC wndproc)
{
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(wcex));
	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_HREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = wndproc;
	wcex.hInstance = instance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.lpszClassName = "Core";

	return RegisterClassEx(&wcex);
}