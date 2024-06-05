// compile with: /D_UNICODE /DUNICODE /DWIN32 /D_WINDOWS /c

#include <windows.h>
#include <stdlib.h>
#include <string>
#include <tchar.h>
#include <wrl.h>
#include <wil/com.h>
#include <filesystem>

// <IncludeHeader>
// include WebView2 header
#include "WebView2.h"
#include "Flags.h"
// </IncludeHeader>

using namespace Microsoft::WRL;

// Global variables


// The main window class name.

// The string that appears in the application's title bar.

HINSTANCE hInst;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Pointer to WebViewController
static wil::com_ptr<ICoreWebView2Controller> webviewController;

// Pointer to WebView window
static wil::com_ptr<ICoreWebView2> webview;

Flags flags;

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	
	for (int i{ 0 }; i < __argc; ++i) {
		if (std::string(__argv[i]) == "-url" && i+1<__argc) {
			flags.url = __argv[i + 1];
		}
		else if (std::string(__argv[i]) == "-width" && i + 1 < __argc) {
			flags.window_width = std::stoi(std::string(__argv[i + 1]));
		}
		else if (std::string(__argv[i]) == "-height" && i + 1 < __argc) {
			flags.window_height = std::stoi(std::string(__argv[i + 1]));
		}
		else if (std::string(__argv[i]) == "-title" && i + 1 < __argc) {
			if (std::string(__argv[i + 1]) == "true") {
				flags.title = true;
			}
			else {
				flags.title = false;
			}
		}
		else if (std::string(__argv[i]) == "-title_text" && i + 1 < __argc) {
			if (flags.title) {
				flags.title_text = __argv[i + 1];
			}
		}
		else if (std::string(__argv[i]) == "-full_screen" && i + 1 < __argc) {
			if (std::string(__argv[i + 1]) == "true") {
				flags.full_screen = true;
			}
			else {
				flags.full_screen = false;
			}
		}
		else if (std::string(__argv[i]) == "-center" && i + 1 < __argc) {
			if (std::string(__argv[i + 1]) == "true") {
				flags.center = true;
			}
			else {
				flags.center = false;
			}
		}
		else if (std::string(__argv[i]) == "-posx" && i + 1 < __argc) {
			flags.positionX = std::stoi(std::string(__argv[i + 1]));
		}
		else if (std::string(__argv[i]) == "-posy" && i + 1 < __argc) {
			flags.positionY = std::stoi(std::string(__argv[i + 1]));
		}
		else if (std::string(__argv[i]) == "-className" && i + 1 < __argc) {
			flags.className = __argv[i + 1];
		}
		else if (std::string(__argv[i]) == "-strict_url" && i + 1 < __argc) {
			if (std::string(__argv[i + 1]) == "true") {
				flags.strictURL = true;
			}
			else {
				flags.strictURL = false;
			}
		}
		else if (std::string(__argv[i]) == "-url_style" && i + 1 < __argc) {
			flags.url_style = __argv[i + 1];
		}
	}

	


	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;

	int wideSize = MultiByteToWideChar(CP_UTF8, 0, flags.className.c_str(), -1, nullptr, 0);
	std::wstring classname(wideSize, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, flags.className.c_str(), -1, &classname[0], wideSize);
	wcex.lpszClassName = classname.c_str();
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);

		return 1;
	}

	// Store instance handle in our global variable
	hInst = hInstance;

	// The parameters to CreateWindow explained:
	// szWindowClass: the name of the application
	// szTitle: the text that appears in the title bar
	// WS_OVERLAPPEDWINDOW: the type of window to create
	// CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
	// 500, 100: initial size (width, length)
	// NULL: the parent of this window
	// NULL: this application does not have a menu bar
	// hInstance: the first parameter from WinMain
	// NULL: not used in this application
	wideSize = MultiByteToWideChar(CP_UTF8, 0, flags.title_text.c_str(), -1, nullptr, 0);
	std::wstring title_text(wideSize, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, flags.title_text.c_str(), -1, &title_text[0], wideSize);
	
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	HWND hWnd = CreateWindow(
		classname.c_str(),
		title_text.c_str(),
		[&]() -> int{
			if (!flags.title) {
				return WS_POPUP | WS_VISIBLE;
			}
			else {
				return WS_OVERLAPPEDWINDOW;
			}
		}(), // Add WS_POPUP to remove title bar
		[&]()-> int {
			if (flags.center) {
				return (screenWidth - flags.window_width) / 2;
			}
			else {
				return flags.positionX;
			}
		}(), 
		[&]()-> int {
			if (flags.center) {
				return (screenHeight - flags.window_height) / 2;
			}
			else {
				return flags.positionY;
			}
		}(), // Set position to (0, 0)
		flags.window_width, flags.window_height, // Set window width and height
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (flags.full_screen) {
		// Set the window position and size to cover the entire screen
		SetWindowPos(hWnd, NULL, 0, 0, screenWidth, screenHeight, SWP_FRAMECHANGED);

		// Maximize the window
		ShowWindow(hWnd, SW_MAXIMIZE);
	}
	
	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);

		return 1;
	}

	// The parameters to ShowWindow explained:
	// hWnd: the value returned from CreateWindow
	// nCmdShow: the fourth parameter from WinMain
	ShowWindow(hWnd,
		nCmdShow);
	UpdateWindow(hWnd);

	// <-- WebView2 sample code starts here -->
	// Step 3 - Create a single WebView within the parent window
	// Locate the browser and set up the environment for WebView
	CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, nullptr,
		Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
			[hWnd](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {

				// Create a CoreWebView2Controller and get the associated CoreWebView2 whose parent is the main window hWnd
				env->CreateCoreWebView2Controller(hWnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
					[hWnd](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
						if (controller != nullptr) {
							webviewController = controller;
							webviewController->get_CoreWebView2(&webview);
						}

						// Add a few settings for the webview
						// The demo step is redundant since the values are the default settings
						wil::com_ptr<ICoreWebView2Settings> settings;
						webview->get_Settings(&settings);
						settings->put_IsScriptEnabled(TRUE);
						settings->put_AreDefaultScriptDialogsEnabled(TRUE);
						settings->put_IsWebMessageEnabled(TRUE);

						// Resize WebView to fit the bounds of the parent window
						RECT bounds;
						GetClientRect(hWnd, &bounds);
						webviewController->put_Bounds(bounds);

						
						// Schedule an async task to navigate to Bing
						if (flags.url.length()) {
							auto url = Flags::ConvertCharToWchar(flags.url.c_str());
							webview->Navigate(url);
							delete url;
						}
						else {
							webview->NavigateToString(L"<body><h1>Welcome to Mutexis UI Engine</h1><h2>latest call command for engine: </h2><h2> ./Win_MutexisSDK.exe -url https://skillfulelectro.github.io/Sphere/ -width 680 -height 680 -title true -title_text goz -full_screen false -center false -posx 100 -posy 100 -className your_WindowClassName -strict_url true -url_style https://google.com</h2></body>");
						}

						// <NavigationEvents>
						// Step 4 - Navigation events
						// register an ICoreWebView2NavigationStartingEventHandler to cancel any non-https navigation
						EventRegistrationToken token;
						webview->add_NavigationStarting(Callback<ICoreWebView2NavigationStartingEventHandler>(
							[&](ICoreWebView2* webview, ICoreWebView2NavigationStartingEventArgs* args) -> HRESULT {
								if (flags.strictURL) {
									int wideSize = MultiByteToWideChar(CP_UTF8, 0, flags.url_style.c_str(), -1, nullptr, 0);
									std::wstring url_text(wideSize, L'\0');
									MultiByteToWideChar(CP_UTF8, 0, flags.url_style.c_str(), -1, &url_text[0], wideSize);
									wil::unique_cotaskmem_string uri;
									args->get_Uri(&uri);
									std::wstring source(uri.get());
									if (source.substr(0, flags.url_style.length()) != url_text.c_str()) {
										args->put_Cancel(true);
									}
								}
								return S_OK;
							}).Get(), &token);
						// </NavigationEvents>

						/*
						// <Scripting>
						// Step 5 - Scripting
						// Schedule an async task to add initialization script that freezes the Object object
						webview->AddScriptToExecuteOnDocumentCreated(L"Object.freeze(Object);", nullptr);
						// Schedule an async task to get the document URL
						webview->ExecuteScript(L"window.document.URL;", Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
							[](HRESULT errorCode, LPCWSTR resultObjectAsJson) -> HRESULT {
								LPCWSTR URL = resultObjectAsJson;
								//doSomethingWithURL(URL);
								return S_OK;
							}).Get());
						// </Scripting>

						// <CommunicationHostWeb>
						// Step 6 - Communication between host and web content
						// Set an event handler for the host to return received message back to the web content
						webview->add_WebMessageReceived(Callback<ICoreWebView2WebMessageReceivedEventHandler>(
							[](ICoreWebView2* webview, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT {
								wil::unique_cotaskmem_string message;
								args->TryGetWebMessageAsString(&message);
								// processMessage(&message);
								webview->PostWebMessageAsString(message.get());
								return S_OK;
							}).Get(), &token);

						// Schedule an async task to add initialization script that
						// 1) Add an listener to print message from the host
						// 2) Post document URL to the host
						webview->AddScriptToExecuteOnDocumentCreated(
							L"window.chrome.webview.addEventListener(\'message\', event => alert(event.data));" \
							L"window.chrome.webview.postMessage(window.document.URL);",
							nullptr);
						// </CommunicationHostWeb>

						*/

						return S_OK;
					}).Get());
				return S_OK;
			}).Get());


	
	// <-- WebView2 sample code ends here -->

	// Main message loop:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


	
	return (int)msg.wParam;
}

//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_DESTROY  - post a quit message and return
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	TCHAR greeting[] = _T("Hello, Windows desktop!");

	switch (message)
	{
	case WM_SIZE:
		if (webviewController != nullptr) {
			RECT bounds;
			GetClientRect(hWnd, &bounds);
			webviewController->put_Bounds(bounds);
		};
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}
