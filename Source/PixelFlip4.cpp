
// -------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing
// loop. Idle time is used to render the scene
//
// See Game Coding Complete 4th Edition - Chapter 5, page 134
// -------------------------------------------------------------------------------------

INT WINAPI PixelFlip4(HINSTANCE hInstance,
					  HINSTANCE hPrevInstance,
					  LPWSTR lpCmdLine,
					  int nCmdShow)
{
	// Set up checks for memory leaks.
	// Game Coding Complete reference - Chapter 21, page 834
	//int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

	// set this flag to keep memory blocks around
	// tmpDbgFlag |= _CRTDBG_DELAY_GREE_MEM_DF;			// this flag will cause intermittent pauses in your game and potentially cause it to run out of memory!

	// perform memory check for each alloc/dealloc
	//tmpDbgFlag |= _CRTDBG_CHECK_ALWAYS_DF;			// remember this is VERY VERY SLOW!!

	//_CRTDBG_LEAK_CHECK_DF is used at program initialization to force a
	// leak check just before program exit. This is important because
	// some classes may dynamically allocate memory in globally constructed
	// objects.
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;

	_CrtSetDbgFlag(tmpDbgFlag);

	// [rez] Initialize the logging system as the very first thing you ever do!
	Logger::Init("logging.xml");

	g_pApp->m_Options.Init("PlayerOptions.xml", lpCmdLine);
	
	// Set the callback functions. These functions allow the sample framework to notify
	// the application about device changes, user input, and windows messages. The callbacks
	// are optional so you need only set callbacks for events you're interested
	// in. However, if you don't handle the device reset/lost callbacks then the sample
	// framework won't be able to reset your device since the application must first
	// release all device resources before resetting. Likewise, if you don't handle the
	// device created/destroyed callbacks then the sample frameweork won't be able to
	// recreate you device resources.

	DXUTSetCallbackMsgProc(GameCodeApp::MsgProc);
	DXUTSetCallbackFrameMove(GameCodeApp::OnUpdateGame);
	DXUTSetCallbackDeviceChanging(GameCodeApp::ModifyDeviceSettings);

	if (g_pApp->m_Options.m_Renderer == "Direct3D 9")
	{

	}
	else if ()
	{


	}
	else 
	{


	}

	// Show the cursor and clip it when in full screen
	DXUTSetCursorSettings(true, true);

	// Perform application initialization
	if (!g_pApp->InitInstance(hInstance, lpCmdLine, 0, g_pApp->m_Options.m_ScreenSize.x, g_pApp->m_Options.m_ScreenSize.y))
	{
		// [rez] Note: Fix memory leaks if we hit this branch. Also, print an error.
		return FALSE;
	}

	// Pass control to the sample framework for handling the message pump and
	// dispatching render calls. The sample framework will call your FrameMove
	// and FrameRender callback when there is idle time between  handling window
	// messages

	DXUTMainLoop();
	DXUTShutdown();

	// [rez] Destroy the logging system at the last possible moment
	Logger::Destroy();

	return g_pApp->GetExitCode();
}