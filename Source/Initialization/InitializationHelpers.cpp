#include "InitializationHelpers.h"

bool IsOnlyInstance(const char* gameTitle)
{
	// Find the window. If active, set and return false
	// Only one game isntance may have this mutex at a time...

	HANDLE handle = CreateMutex(nullptr, true, gameTitle);

	// Does anyone else think 'ERROR_SUCCESS' is a bit of an oxymoron?
	if (GetLastError() != ERROR_SUCCESS)
	{
		HWND hWnd = FindWindow(gameTitle, nullptr);
		if (hWnd)
		{
			// An instance of your game is already running.
			ShowWindow(hWnd, SW_SHOWNORMAL);
			SetFocus(hWnd);
			SetForegroundWindow(hWnd);
			SetActiveWindow(hWnd);
			return false;
		}
	}
	return true;
}