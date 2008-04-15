#include "stdafx.h"
#include "Application.h"

Application::Application()
{
}

void Application::Run(BasicWindow *bwWindow)
{
	MSG msg;

	bwWindow->Create();

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, m_hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}
