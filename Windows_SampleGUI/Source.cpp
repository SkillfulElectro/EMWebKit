#include <Windows.h>
#include <iostream>


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char* appPath = "emwebkit";
    
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    if (CreateProcessA(nullptr, const_cast<LPSTR>(appPath), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
        std::cout << "Process started successfully!" << std::endl;
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else {
        std::cerr << "Error creating process. Error code: " << GetLastError() << std::endl;
    }

    
    return 0;
}
