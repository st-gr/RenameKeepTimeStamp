// FileUtils.h
#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <string>

bool RenameFileAndPreserveTimestamps(const std::wstring& oldPath, const std::wstring& newPath);

#endif // FILEUTILS_H
