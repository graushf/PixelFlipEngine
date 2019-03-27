//=================================================================
// App.cpp : Defines the Application Layer for the PixelFlipEngine
// ================================================================

#include "Common/CommonStd.h"

App *g_App = NULL;

// ****************************************************************
//
// App implementation
//
// ****************************************************************

App::App()
{

}

// ****************************************************************
// Win32 Specific Stuff
// 
// InitInstance - this checks system resources, creates your window
// and launches the game
//
// preprocessor macro setting in the C++ options of the project
// provides the base macro C preprocessor string concatenation
// takes care of the rest.
//
// ****************************************************************

bool App::InitInstance(HINSTANCE hInstance, LPWSTR lpCmdLine, HWND hWnd, int screenWidth, int screenHeight)
{
	// Check for existing instance of the same window
	//
#ifndef _DEBUG
	// Note - it can be really useful to debug network code to have
	// more than one instance of the game up at one time - comment 
	// this lines in case you want two instances of the game/engine
	// running at the same time.
	if (!IsOnlyInstance(VGetGameTitle()))
	{
		return false;
	}
#endif

	// We don't need a mouse cursor by default, let the game turn it on.
	SetCursor(nullptr);

	// Check for adequate machine resources
	bool resourceCheck = false;
	while (!resourceCheck)
	{
		const DWORDLONG physicalRam = 512 * MEGABYTE;
		const DWORDLONG virtualRam = 1024 * MEGABYTE;
		const DWORDLONG diskSpace = 10 * MEGABYTE;

		if (!CheckStorage(diskSpace))
		{
			return false;
		}

		const DWORD minCpuSpeed = 1300;			// 1.3GHz
		DWORD thisCPU = ReadCPUSpeed();
		if (thisCPU < minCpuSpeed)
		{
			GCC_ERROR("GetCPUSpeed reports CPU is too slow for this game");
			return false;
		}

		resourceCheck = true;
	}

	m_hInstance = hInstance;

	// register all events
	RegisterEngineEvents();
	VRegisterGameEvents();

	//
	// Initialize the ResourceCache
	//
	//		Note - This a little different from the book. Here we have a special resource ZIP file class, DevelopmentResourceZipFile,
	//		that actually reads directly from the source asset files, rather than the ZIP file. This is MUCH better during development,
	//		since you don't have to rebuild the ZIP file every time you manke a minor change to an asset.
	IResourceFile* zipFile = (m_bIsEditorRunning || m_Options.m_useDevelopmentDirectories) ?
		GCC_NEW DevelopmentResourceZipFile(L"Assets.zip", DevelopmentResourceZipFile::Editor) :
		GCC_NEW ResourceZipFile(L"Assets.zip");

	m_ResCache = GCC_NEW ResCache(50, zipFile);

	if (!m_ResCache->Init())
	{
		GCC_ERROR("Failed to initialize resource cache! Are your paths set up correctly?");
		return false;
	}

	extern shared_ptr<IResourceLoader> CreateWAVResourceLoader();
	extern shared_ptr<IResourceLoader> CreateOGGResourceLoader();
	extern shared_ptr<IResourceLoader> CreateDDSResourceLoader();
	extern shared_ptr<IResourceLoader> CreateJPGResourceLoader();
	extern shared_ptr<IResourceLoader> CreateXmlResourceLoader();
	extern shared_ptr<IResourceLoader> CreateSdkMeshResourceLoader();
	extern shared_ptr<IResourceLoader> CreateScriptResourceLoader();

	// Note - register these in order from least specific to most specific! They get pushed onto a list.
	// RegisterLoader is discussed in Chapter 5, page 142
	m_ResCache->RegisterLoader(CreateWAVResourceLoader());
	m_ResCache->RegisterLoader(CreateOGGResourceLoader());
	m_ResCache->RegisterLoader(CreateDDSResourceLoader());
	m_ResCache->RegisterLoader(CreateJPGResourceLoader());
	m_ResCache->RegisterLoader(CreateXmlResourceLoader());
	m_ResCache->RegisterLoader(CreateSdkMeshResourceLoader());
	m_ResCache->RegisterLoader(CreateScriptResourceLoader());

	if (!LoadStrings("English"))
	{
		GCC_ERROR("Failed to load strings");
		return false;
	}

	// Rez - Up the Lua State manager now, and run the initial script - Discussed in Chapter 5, page 144.
	if (!LuaStateManager::Create())
	{
		GCC_ERROR("Failed to initialize Lua");
		return false;
	}

	// Load the preinit file. This is within braces to create a scope and destroy the resource once it's done. We
	// don't need to do anything with it, we just need to load it.
	{
		Resource resource(SCRIPT_PREINIT_FILE);
		shared_ptr<ResHandle> pResourceHandle = m_ResCache->GetHandle(&resource); // this actually loads the XML file form the zip file
	}

	// Register function exported from C++
	ScriptExports::Register();
	ScriptProcess::RegisterScriptClass();
	BaseScriptComponent::RegisterScriptFunctions();

	// The event manager should be created next so that subsystems can hook in as desired.
	// Discussed in Chapter 5, page 144.
	m_pEventManager = GCC_NEW EventManager("GameCodeApp Event Mgr", true);
	if (!m_pEventManager)
	{
		GCC_ERROR("Failed to create EventManager.");
		return false;
	}

	// DXUTInit, DXUTCreateWindow - Chapter 5, page 145
	DXUTInit(true, true, lpCmdLine, true); // Parse the command line, handle the default hotkeys, and show msgboxes

	if (hWnd == nullptr)
	{
		DXUTCreateWindow(VGetGameTitle(), hInstance, VGetIcon());
	}
	else
	{
		DXUTSetWindow(hWnd, hWnd, hWnd);
	}

	if (!GetHwnd())
	{
		return FALSE;
	}
	SetWindowText(GetHwnd(), VGetGameTitle());

	// initialize the directory location you can store save game files
	_tcscpy_s(m_saveGameDirectory, GetSaveGameDirectory(GetHwnd(), VGetGameAppDirectory()));

	// DXUTCreateDevice - Chapter 5 - page 139
	m_screenSize = Point(screenWidth, screenHeight);

	// If you have an older video card that only supports D3D9, comment in the next line, and make sure
	// the renderer setting in Game\PlayerOptions.xml is set to "Direct3D 9"
	// DXUTCreateDevice( D3D_FEATURE_LEVEL_9_3, true, screenWidth, screenHeight);
	DXUTCreateDevice(D3D_FEATURE_LEVEL_10_1, true, screenWidth, screenHeight);

	if (GetRendererImpl() == Renderer_D3D9)
	{
		m_Renderer = shared_ptr<IRenderer>(GCC_NEW D3DRenderer9());
	}
	else if (GetRendererImp() == Renderer_D3D11)
	{
		m_Renderer = shared_ptr<IRenderer>(GCC_NEW D3DRenderer11());
	}
	m_Renderer->VSetBackgroundColor(255, 20, 20, 200);
	m_Renderer->VOnRestore();

	// You usually must have an HWND to initialize your game views...
	// VCreateGameAndView		- Chapter 5, page 145
	m_pGame = VCreateGameAndView();
	if (!m_pGame)
	{
		return false;
	}

	// now that all the major systems are initialized, preload resources
	//		Preload calls are discussed in Chapter 5, page 148
	m_ResCache->Preload("*.ogg", nullptr);
	m_ResCache->Preload("*.dds", nullptr);
	m_ResCache->Preload("*.jpg", nullptr);

	if (App::GetRendererImpl() == App::Renderer_D3D11)
	{
		m_ResCache->Preload("*.sdkmesh", nullptr);
	}

	CheckForJoystick(GetHwnd());

	m_bIsRunning = true;

	return true;
}