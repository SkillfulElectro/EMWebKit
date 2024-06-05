#include <windows.h>
#include <stdlib.h>
#include <string>
#include <tchar.h>
#include <wrl.h>
#include <wil/com.h>
#include <filesystem>


#include "WebView2.h"
#include "Flags.h"


using namespace Microsoft::WRL;


HINSTANCE hInst;


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


static wil::com_ptr<ICoreWebView2Controller> webviewController;

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

	
	hInst = hInstance;

	
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
		}(), 
		flags.window_width, flags.window_height, 
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (flags.full_screen) {
		
		SetWindowPos(hWnd, NULL, 0, 0, screenWidth, screenHeight, SWP_FRAMECHANGED);

		
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

	
	ShowWindow(hWnd,
		nCmdShow);
	UpdateWindow(hWnd);

	
	CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, nullptr,
		Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
			[hWnd](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {

				
				env->CreateCoreWebView2Controller(hWnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
					[hWnd](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
						if (controller != nullptr) {
							webviewController = controller;
							webviewController->get_CoreWebView2(&webview);
						}

						
						wil::com_ptr<ICoreWebView2Settings> settings;
						webview->get_Settings(&settings);
						settings->put_IsScriptEnabled(TRUE);
						settings->put_AreDefaultScriptDialogsEnabled(TRUE);
						settings->put_IsWebMessageEnabled(TRUE);

						
						RECT bounds;
						GetClientRect(hWnd, &bounds);
						webviewController->put_Bounds(bounds);

						
						
						if (flags.url.length()) {
							auto url = Flags::ConvertCharToWchar(flags.url.c_str());
							webview->Navigate(url);
							delete url;
						}
						else {
							webview->NavigateToString(L"<body><h1>Welcome to Mutexis UI Engine</h1><h2>latest call command for engine: </h2><h2> ./Win_MutexisSDK.exe -url https://skillfulelectro.github.io/Sphere/ -width 680 -height 680 -title true -title_text goz -full_screen false -center false -posx 100 -posy 100 -className your_WindowClassName -strict_url true -url_style https://google.com</h2></body>");
						}

						
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
						

						return S_OK;
					}).Get());
				return S_OK;
			}).Get());


	
	
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


	
	return (int)msg.wParam;
}


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
