#pragma once

#include "stdafx.h"
#include "Dialog.h"

class ChatClient;
class Net;
class IRC;

class ConnectDialog : public Dialog
{
public:
	ConnectDialog(HINSTANCE, INT, HWND, ChatClient*);

	INT_PTR CALLBACK HandleMessage(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
private:
	ChatClient *m_bwChatClient;
	Net *m_net;
	IRC *m_irc;
};

