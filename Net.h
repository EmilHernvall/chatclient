
#pragma once

class ChatClient;

class Net 
{
public:
	static BOOL Initialize();
	static VOID CleanUp();

	Net();

	BOOL Connect(LPTSTR szHost, WORD wPort);
	VOID Disconnect();

	BOOL IsConnected() {
		return m_bConnected; 
	}
	LPTSTR GetHost() { return m_szHost; }
	WORD GetPort() { return m_wPort; }

	INT Write(LPTSTR szData);
	INT Read(LPTSTR* ret);

private:
	BOOL m_bConnected;

	SOCKET m_sock;
	LPTSTR m_szHost;
	WORD m_wPort;
};

