#include "stdafx.h"
#include "Dialog.h"
#include "AboutDialog.h"

AboutDialog::AboutDialog(HINSTANCE hInst, INT nResource, HWND hwndParent)
: Dialog(hInst, nResource, hwndParent)
{

}

// Message handler for about box.
INT_PTR CALLBACK AboutDialog::HandleMessage(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
