#include "FileUtils.h"
#include <windows.h>
#include <string>
#include <shlwapi.h>  // For PathRemoveFileSpec
#include <shellapi.h> // For CommandLineToArgvW

#pragma comment(lib, "shlwapi.lib")

// Global variables
std::wstring g_oldFilePath;
std::wstring g_directoryPath;
std::wstring g_oldFileName;
std::wstring g_newFileName;

// Function prototypes
INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
bool RenameFileAndPreserveTimestamps(const std::wstring& oldPath, const std::wstring& newPath);

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int)
{
    // Parse command-line arguments
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    if (argc < 2)
    {
        MessageBoxW(NULL, L"No file specified.", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    g_oldFilePath = argv[1];

    // Check if the file exists
    DWORD fileAttributes = GetFileAttributesW(g_oldFilePath.c_str());
    if (fileAttributes == INVALID_FILE_ATTRIBUTES && GetLastError() == ERROR_FILE_NOT_FOUND)
    {
        MessageBoxW(NULL, L"The specified file does not exist.", L"Error", MB_OK | MB_ICONERROR);
        LocalFree(argv);
        return 1; // Return a non-zero error code
    }

    // Extract directory and file name
    wchar_t directory[MAX_PATH];
    wcscpy_s(directory, MAX_PATH, g_oldFilePath.c_str());
    PathRemoveFileSpecW(directory);
    g_directoryPath = directory;

    wchar_t fileName[MAX_PATH];
    const wchar_t* pFileName = PathFindFileNameW(g_oldFilePath.c_str());
    wcscpy_s(fileName, MAX_PATH, pFileName);
    g_oldFileName = fileName;

    // Show dialog
    DialogBox(hInstance, MAKEINTRESOURCE(101), NULL, DialogProc);

    LocalFree(argv);
    return 0;
}

INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        SetDlgItemTextW(hwndDlg, 1001, g_oldFileName.c_str());

        // Position the dialog centered at the mouse cursor
        POINT cursorPos;
        GetCursorPos(&cursorPos);

        // Get the monitor where the cursor is located
        HMONITOR hMonitor = MonitorFromPoint(cursorPos, MONITOR_DEFAULTTONEAREST);

        // Get monitor information
        MONITORINFO monitorInfo;
        monitorInfo.cbSize = sizeof(MONITORINFO);
        GetMonitorInfo(hMonitor, &monitorInfo);

        // Get the work area of the monitor (excluding taskbar)
        RECT workArea = monitorInfo.rcWork;

        // Get the size of the dialog
        RECT dlgRect;
        GetWindowRect(hwndDlg, &dlgRect);
        int dlgWidth = dlgRect.right - dlgRect.left;
        int dlgHeight = dlgRect.bottom - dlgRect.top;

        // Calculate the desired position to center the dialog at the cursor position
        int xPos = cursorPos.x - dlgWidth / 2;
        int yPos = cursorPos.y - dlgHeight / 2;

        // Adjust position if the dialog would be off-screen
        if (xPos < workArea.left)
            xPos = workArea.left;
        else if (xPos + dlgWidth > workArea.right)
            xPos = workArea.right - dlgWidth;

        if (yPos < workArea.top)
            yPos = workArea.top;
        else if (yPos + dlgHeight > workArea.bottom)
            yPos = workArea.bottom - dlgHeight;

        // Move the dialog to the calculated position
        SetWindowPos(hwndDlg, NULL, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

        return TRUE;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
        {
            wchar_t newFileName[MAX_PATH];
            GetDlgItemTextW(hwndDlg, 1001, newFileName, MAX_PATH);
            g_newFileName = newFileName;

            // Build new file path
            std::wstring newFilePath = g_directoryPath + L"\\" + g_newFileName;

            // Rename and preserve timestamps
            if (RenameFileAndPreserveTimestamps(g_oldFilePath, newFilePath))
            {
                EndDialog(hwndDlg, IDOK);
            }
            else
            {
                MessageBoxW(hwndDlg, L"Failed to rename the file.", L"Error", MB_OK | MB_ICONERROR);
                EndDialog(hwndDlg, IDCANCEL);
            }
            return TRUE;
        }
        case IDCANCEL:
            EndDialog(hwndDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}


