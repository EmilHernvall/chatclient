#pragma once

class Net;

class IRC
{
public:
	IRC(Net *net);

	VOID SetNick(LPTSTR val) { m_szNick = val; }
	VOID SetUser(LPTSTR val) { m_szUser = val; }
	VOID SetChannel(LPTSTR val) { m_szChannel = val; }

	LPTSTR GetNick() { return m_szNick; }
	LPTSTR GetUser() { return m_szUser; }
	LPTSTR GetChannel() { return m_szChannel; }
private:
	Net *m_net;	

	LPTSTR m_szNick;
	LPTSTR m_szUser;
	LPTSTR m_szChannel;
};

