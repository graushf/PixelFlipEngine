#pragma once

#include "Common/CommonStd.h"

//=================================================================
// App.h : Defines the entry point for the application.
// ================================================================

class App {
protected:

public:

protected:

public:
	App(); // Constructor

	// Game Application Data
	// You must define these in an inherited
	// class - See TeapotWarsApp for an example
	virtual TCHAR *VGetGameTitle()=0;
	virtual TCHAR *VGetGameAppDirectory()=0;
	virtual HICON VGetIcon()=0;

	// Win32 Specific Stuff
	HWND GetHwnd();
	HINSTANCE GetInstance() { return m_hInstance; }
	virtual bool InitInstance(HINSTANCE hInstance, LPWSTR lpCmdLine, HWND hWnd = NULL, int screenWidth = SCREEN_WIDTH, int screenHeight = SCREEN_HEIGHT);

protected:

public:

protected:

private:

};

extern App *g_pApp;
