#ifndef __UTILS_H_
#define __UTILS_H_

#include <Windows.h>
#include <filesystem>
#include <string>


#define ReleaseCOM(x) { if(x) { x->Release(); x = 0; } }

namespace fs = std::filesystem;

LPCSTR ConvertWideToANSI(LPCWSTR wideStr);
std::wstring findFullPath(const std::string& targetFileName, bool searchInHidden = false);
std::string WStringToUTF8(const std::wstring& wstr);

#endif	// __UTILS_H_	
