#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <cstdlib> // for getenv, setenv, _putenv_s

#ifdef _WIN32
#include <windows.h>
#include <tlhelp32.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <csignal>
#include <cstring>
#include <dirent.h> 
#include <fstream>
#include <algorithm>
#include <libgen.h>
// extern char **environ; // For accessing environment variables
#endif


namespace nbc {
namespace utils {
// 获取当前工作目录（跨平台）
std::string getCurrentWorkingDirectory() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    if (GetCurrentDirectoryA(MAX_PATH, buffer)) {
        return std::string(buffer);
    } else {
        return std::string();
    }
#else
    char buffer[PATH_MAX];
    if (getcwd(buffer, sizeof(buffer)) != nullptr) {
        return std::string(buffer);
    } else {
        return std::string();
    }
#endif
}
bool isProcessRunning(const std::string& processName) {
#ifdef _WIN32
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return false;
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hSnapshot, &pe32)) {
        CloseHandle(hSnapshot); return false;
    }
    do {
        if (_stricmp(pe32.szExeFile, processName.c_str()) == 0) {
            CloseHandle(hSnapshot); return true;
        }
    } while (Process32Next(hSnapshot, &pe32));
    CloseHandle(hSnapshot);
    return false;
#else
    DIR* dir = opendir("/proc");
    if (dir == nullptr) return false;
    struct dirent* entry;
    bool found = false;
    while ((entry = readdir(dir)) != nullptr) {
        std::string pid_str = entry->d_name;
        if (std::all_of(pid_str.begin(), pid_str.end(), ::isdigit)) {
            std::string comm_path = "/proc/" + pid_str + "/comm";
            std::ifstream comm_file(comm_path);
            if (comm_file.is_open()) {
                std::string name;
                std::getline(comm_file, name);
                if (name == processName) {
                    found = true;
                    break;
                }
            }
        }
    }
    closedir(dir);
    return found;
#endif
}


std::string getProcessName(const std::string& fullPath) {
#ifdef _WIN32
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];
    _splitpath_s(fullPath.c_str(), NULL, 0, NULL, 0, fname, _MAX_FNAME, ext, _MAX_EXT);
    return std::string(fname) + std::string(ext);
#else
    char* path_copy = strdup(fullPath.c_str());
    char* name = basename(path_copy);
    std::string result(name);
    free(path_copy);
    return result;
#endif
}

// 打印当前工作目录
void printCurrentWorkingDirectory() {
    std::string cwd = getCurrentWorkingDirectory();
    if (!cwd.empty()) {
        std::cout << "当前工作目录: " << cwd << std::endl;
    } else {
        std::cerr << "无法获取当前工作目录" << std::endl;
    }
}

}
}