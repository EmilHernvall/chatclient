
#pragma once

#include "BasicWindow.h"

class Application
{
public:
	Application();
	void Run(BasicWindow *bwWindow);

private:
	HACCEL m_hAccelTable;
};
