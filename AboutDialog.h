
#pragma once

#include "Dialog.h"

class AboutDialog : public Dialog
{
public:
	AboutDialog(HINSTANCE, INT, HWND);

	INT_PTR CALLBACK HandleMessage(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
};
