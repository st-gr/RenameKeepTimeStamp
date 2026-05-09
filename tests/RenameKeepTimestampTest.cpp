#include "gtest/gtest.h"
#include <windows.h>
#include <string>
#include <fstream>
#include <shlwapi.h>

bool RenameFileAndPreserveTimestamps(const std::wstring& oldPath, const std::wstring& newPath);

TEST(RenameKeepTimestampTest, RenameAndPreserveTimestamps)
{
    // Setup: Create a temporary file
    wchar_t tempPath[MAX_PATH];
    GetTempPath(MAX_PATH, tempPath);

    std::wstring tempFile = std::wstring(tempPath) + L"temp_test_file.txt";
    std::wofstream ofs(tempFile);
    ofs << L"Test content";
    ofs.close();

    // Get original timestamps
    HANDLE hFile = CreateFile(tempFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    ASSERT_NE(hFile, INVALID_HANDLE_VALUE);

    FILETIME creationTime, lastAccessTime, lastWriteTime;
    BOOL success = GetFileTime(hFile, &creationTime, &lastAccessTime, &lastWriteTime);
    CloseHandle(hFile);
    ASSERT_TRUE(success);

    // Rename the file
    std::wstring newFileName = std::wstring(tempPath) + L"temp_test_file_renamed.txt";
    bool result = RenameFileAndPreserveTimestamps(tempFile, newFileName);
    EXPECT_TRUE(result);

    // Verify timestamps
    hFile = CreateFile(newFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    ASSERT_NE(hFile, INVALID_HANDLE_VALUE);

    FILETIME newCreationTime, newLastAccessTime, newLastWriteTime;
    success = GetFileTime(hFile, &newCreationTime, &newLastAccessTime, &newLastWriteTime);
    CloseHandle(hFile);
    ASSERT_TRUE(success);

    EXPECT_EQ(0, memcmp(&creationTime, &newCreationTime, sizeof(FILETIME)));
    EXPECT_EQ(0, memcmp(&lastAccessTime, &newLastAccessTime, sizeof(FILETIME)));
    EXPECT_EQ(0, memcmp(&lastWriteTime, &newLastWriteTime, sizeof(FILETIME)));

    // Cleanup
    DeleteFile(newFileName.c_str());
}

TEST(RenameKeepTimestampTest, RenameInSameDirectory)
{
    // Simulates CLI mode: rename with just a new filename in the same directory
    wchar_t tempPath[MAX_PATH];
    GetTempPath(MAX_PATH, tempPath);

    std::wstring tempFile = std::wstring(tempPath) + L"cli_test_original.txt";
    std::wofstream ofs(tempFile);
    ofs << L"CLI test content";
    ofs.close();

    std::wstring newFile = std::wstring(tempPath) + L"cli_test_newname.txt";
    // Ensure target doesn't already exist
    DeleteFile(newFile.c_str());

    bool result = RenameFileAndPreserveTimestamps(tempFile, newFile);
    EXPECT_TRUE(result);

    // Verify old file is gone and new file exists
    EXPECT_EQ(INVALID_FILE_ATTRIBUTES, GetFileAttributesW(tempFile.c_str()));
    EXPECT_NE(INVALID_FILE_ATTRIBUTES, GetFileAttributesW(newFile.c_str()));

    // Cleanup
    DeleteFile(newFile.c_str());
}

TEST(RenameKeepTimestampTest, NonExistentFile)
{
    std::wstring nonExistentFile = L"C:\\NonExistentFile.txt";

    // Simulate passing the non-existent file to the program
    DWORD fileAttributes = GetFileAttributesW(nonExistentFile.c_str());
    ASSERT_EQ(fileAttributes, INVALID_FILE_ATTRIBUTES);
    ASSERT_EQ(GetLastError(), ERROR_FILE_NOT_FOUND);
}

TEST(RenameKeepTimestampTest, RenameNonExistentFileFails)
{
    std::wstring nonExistentFile = L"C:\\NonExistentFile_12345.txt";
    std::wstring newFile = L"C:\\ShouldNotExist_12345.txt";

    bool result = RenameFileAndPreserveTimestamps(nonExistentFile, newFile);
    EXPECT_FALSE(result);
}
