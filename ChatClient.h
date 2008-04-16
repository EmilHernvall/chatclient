#pragma once

#include "resource.h"

#define ID_BUFFER	1
#define ID_INPUT	2

#define ID_CHANNELS	50
#define ID_CHANNEL_STATUS	(ID_CHANNELS+0)

#define MAX_LOADSTRING 100

#include "BasicWindow.h"
#include "SubclassedWindow.h"

class AboutDialog;
class ConnectDialog;
class Net;
class IRC;
class BufferEdit;
class InputEdit;

class ChatClient : public BasicWindow
{
public:
	ChatClient(HINSTANCE hInst, INT nCmdShow, LPCWSTR szWindowClass, LPCWSTR szWindowTitle);
	LRESULT CALLBACK HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void AppendToBuffer(LPTSTR szData);
	void ParseCommand(LPTSTR szCommand);

	Net *GetNet() { return m_net; }
	IRC *GetIRC() { return m_irc; }

private:
	BufferEdit *m_bufferEdit;
	InputEdit *m_inputEdit;
	Net *m_net;
	IRC *m_irc;
	AboutDialog *m_about;
	ConnectDialog *m_connect;
};

class BufferEdit : public SubclassedWindow
{
public:
	BufferEdit(HWND hwndParent, HINSTANCE hInstance);
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
