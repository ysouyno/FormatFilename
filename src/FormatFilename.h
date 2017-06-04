#pragma once

#if defined(_MSC_VER)
#include <windows.h>
#pragma warning(disable: 4996)
#if _MSC_VER >= 1900
#include <filesystem>
#else
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#endif
#else
#include <sys/stat.h>
#endif

#include <iostream>
#include <string>
#include <algorithm>
#include <cstdio>

std::wstring s2ws(const std::string & str);
std::string ws2s(const std::wstring & w_str);
bool file_is_exists(const std::wstring & path_file);
bool file_is_directory(const std::wstring & path_file);
void rename_file(const std::wstring & path_file);
void rename_dirs_and_files(const std::wstring & path_file);
