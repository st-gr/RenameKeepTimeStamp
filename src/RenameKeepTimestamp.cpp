#include "FileUtils.h"
#include <windows.h>
#include <string>
#include <cstdio>
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

void AttachParentConsole()
{
    if (AttachConsole(ATTACH_PARENT_PROCESS))
    {
        FILE* fp;
        _wfreopen_s(&fp, L"CONOUT$", L"w", stdout);
        _wfreopen_s(&fp, L"CONOUT$", L"w", stderr);
    }
}

void PrintUsage()
{
    wprintf(L"Usage: RenameKeepTimestamp.exe [options] <filepath> [<newname>]\n");
    wprintf(L"\n");
    wprintf(L"  <filepath>   Path to the file to rename\n");
    wprintf(L"  <newname>    New filename (without path). If omitted, a dialog is shown.\n");
    wprintf(L"\n");
    wprintf(L"Options:\n");
    wprintf(L"  -h, --help   Show this help message\n");
    wprintf(L"\n");
    wprintf(L"Exit codes:\n");
    wprintf(L"  0  Success\n");
    wprintf(L"  1  Error (file not found, rename failed, etc.)\n");
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int)
{
    // Parse command-line arguments
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    // No arguments: print usage
    if (argc < 2)
    {
        AttachParentConsole();
        PrintUsage();
        LocalFree(argv);
        return 1;
    }

    // Check for help flag
    std::wstring firstArg = argv[1];
    if (firstArg == L"--help" || firstArg == L"-h")
    {
        AttachParentConsole();
        PrintUsage();
        LocalFree(argv);
        return 0;
    }

    g_oldFilePath = argv[1];

    // Check if the file exists
    DWORD fileAttributes = GetFileAttributesW(g_oldFilePath.c_str());
    if (fileAttributes == INVALID_FILE_ATTRIBUTES && GetLastError() == ERROR_FILE_NOT_FOUND)
    {
        if (argc >= 3)
        {
            AttachParentConsole();
            fwprintf(stderr, L"Error: The specified file does not exist: %s\n", g_oldFilePath.c_str());
        }
        else
        {
            MessageBoxW(NULL, L"The specified file does not exist.", L"Error", MB_OK | MB_ICONERROR);
        }
        LocalFree(argv);
        return 1;
    }

    // Extract directory and file name
    wchar_t directory[MAX_PATH];
    wcscpy_s(directory, MAX_PATH, g_oldFilePath.c_str());
    PathRemoveFileSpecW(directory);
    g_directoryPath = directory;

    const wchar_t* pFileName = PathFindFileNameW(g_oldFilePath.c_str());
    g_oldFileName = pFileName;

    // CLI mode: new name provided as argument
    if (argc >= 3)
    {
        AttachParentConsole();
        g_newFileName = argv[2];

        std::wstring newFilePath = g_directoryPath + L"\\" + g_newFileName;

        if (RenameFileAndPreserveTimestamps(g_oldFilePath, newFilePath))
        {
            wprintf(L"Renamed: %s -> %s\n", g_oldFileName.c_str(), g_newFileName.c_str());
            LocalFree(argv);
            return 0;
        }
        else
        {
            fwprintf(stderr, L"Error: Failed to rename the file.\n");
            LocalFree(argv);
            return 1;
        }
    }

    // GUI mode: show dialog
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
