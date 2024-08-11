#include <windows.h>
#include <shellapi.h>
#include <fstream>

// Define an ID for the system tray icon
#define ID_TRAY_ICON 1

// Define the path to the text file
#define FILE_PATH "clickfolders.txt"

// Function to read the text file and get the folder paths
bool getFolderPaths(std::string folderPaths[12]) { // ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    std::ifstream file(FILE_PATH);
    if (!file.is_open()) {
        return false;
    }
    for (int i = 0; i < 12; i++) { //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
        if (!std::getline(file, folderPaths[i])) {
            return false;
        }
    }
    return true;
}

// Window procedure for the hidden window
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        // Create the system tray icon
        {
            NOTIFYICONDATA nid;
            ZeroMemory(&nid, sizeof(nid));
            nid.cbSize = sizeof(nid);
            nid.hWnd = hWnd;
            nid.uID = ID_TRAY_ICON;
            nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
            nid.uCallbackMessage = WM_USER + 1;
            nid.hIcon = (HICON)LoadImage(GetModuleHandle(NULL), "icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
            lstrcpy(nid.szTip, "FolderTrayOpener - Esc + click to close");
            Shell_NotifyIcon(NIM_ADD, &nid);
        }
        // Hide the window
        ShowWindow(hWnd, SW_HIDE);
        break;
    case WM_USER + 1:
        // Handle system tray icon messages
        switch (lParam) {
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
            {
                std::string folderPaths[12]; //||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
                if (!getFolderPaths(folderPaths)) {
                    // Error reading the text file
                    MessageBox(NULL, "Error reading the text file.", "Error", MB_ICONERROR | MB_OK);
                    break;
                }
                int index;
                switch (lParam) {
                case WM_LBUTTONDOWN:
                    index = 0;
                    break;
                case WM_RBUTTONDOWN:
                    index = 1;
                    break;
                case WM_MBUTTONDOWN:
                    index = 2;
                    break;
                }
                if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                    // ESC key is being held down, close the app
                    PostQuitMessage(0);
                }
				if (GetAsyncKeyState(VK_MENU) & 0x8000) {
					index += 3;
				}
				if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
					index += 6;
				}
				if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
					index += 9;
				}
				//
                // Open the folder...I am extremely mad. The below line used to be an else statement, and because it was below the shift key, the last one in this stack, it didn't ever activate. FML. and fuck dumb fucking chatgpt. And fuck Coding!
                ShellExecute(NULL, "open", folderPaths[index].c_str(), NULL, NULL, SW_SHOWDEFAULT);
            }
            break;
        }
        break;
    case WM_DESTROY:
        // Remove the system tray icon and exit the app
        {
            NOTIFYICONDATA nid;
            ZeroMemory(&nid, sizeof(nid));
            nid.cbSize = sizeof(nid);
            nid.hWnd = hWnd;
            nid.uID = ID_TRAY_ICON;
            Shell_NotifyIcon(NIM_DELETE, &nid);
            PostQuitMessage(0);
        }
        break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register the window class
    WNDCLASS wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "ClickFoldersWndClass";
    RegisterClass(&wc);

    // Create the hidden window
    HWND hWnd = CreateWindow(wc.lpszClassName, NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, hInstance, NULL);

    // Hide the window
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    // Run the message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}


