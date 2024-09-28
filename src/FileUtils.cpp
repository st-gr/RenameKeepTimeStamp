// FileUtils.cpp
#include "FileUtils.h"
#include <windows.h>

bool RenameFileAndPreserveTimestamps(const std::wstring& oldPath, const std::wstring& newPath)
{
    // Get original file times
    HANDLE hFile = CreateFileW(oldPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return false;

    FILETIME creationTime, lastAccessTime, lastWriteTime;
    if (!GetFileTime(hFile, &creationTime, &lastAccessTime, &lastWriteTime))
    {
        CloseHandle(hFile);
        return false;
    }
    CloseHandle(hFile);

    // Rename the file
    if (!MoveFileW(oldPath.c_str(), newPath.c_str()))
    {
        return false;
    }

    // Set original file times to the new file
    hFile = CreateFileW(newPath.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return false;

    bool result = SetFileTime(hFile, &creationTime, &lastAccessTime, &lastWriteTime);
    CloseHandle(hFile);

    return result;
}
