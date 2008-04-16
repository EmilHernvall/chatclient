#pragma once

#include "resource.h"

#define ID_INPUT	2

#define ID_SUBWINDOW			200
#define ID_SUBWINDOW_STATUS		(ID_SUBWINDOW+0)

#define WM_BASICWINDOW_CREATEWINDOW	(WM_USER+1)

#define MAX_LOADSTRING 100

#include "BasicWindow.h"
#include "SubclassedWindow.h"

class AboutDialog;
class ConnectDialog;
class Net;
class IRC;
class BufferEdit;
class InputEdit;

struct SubWindow {
	INT id;
	HWND hwndButton;
	BufferEdit *buffer;
	LPTSTR szName;
};

class ChatClient : public BasicWindow
{
public:
	ChatClient(HINSTANCE hInst, INT nCmdShow, LPCWSTR szWindowClass, LPCWSTR szWindowTitle);
	LRESULT CALLBACK HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void AppendToBuffer(INT nIndex, LPTSTR szData);
	void ParseCommand(LPTSTR szCommand);

	INT GetWindowNameByID(LPWSTR szName);

	Net *GetNet() { return m_net; }
	IRC *GetIRC() { return m_irc; }

	BufferEdit *GetBufferEdit() { return m_currentSubWindow->buffer; }

private:
	BufferEdit *m_bufferStatus;
	InputEdit *m_inputEdit;
	Net *m_net;
	IRC *m_irc;
	AboutDialog *m_about;
	ConnectDialog *m_connect;
	HFONT m_font;

	struct SubWindow *m_subWindows, *m_currentSubWindow;
	INT m_nSubWindowCount, m_nSubWindowSize;

	INT CreateSubWindow(LPWSTR szName);
};

class BufferEdit : public SubclassedWindow
{
public:
	BufferEdit(HWND hwndParent, HINSTANCE hInstance, INT nId);
	LRESULT CALLBACK HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

class InputEdit : public SubclassedWindow
{
public:
	InputEdit(HWND hwndParent, HINSTANCE hInstance, ChatClient *bwChatClient);
	LRESULT CALLBACK HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
	ChatClient *m_bwChatClient;
};
