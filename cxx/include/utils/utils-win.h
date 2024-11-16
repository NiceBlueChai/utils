#pragma once

#include <string>
#include <Windows.h>

namespace nbc {
namespace utils {

#if defined(UNICODE) || defined(_UNICODE)
DWORD runShellAsAdministrator(LPCWSTR cmd, LPCWSTR arg, int n_show);
#else
DWORD runShellAsAdministrator(LPCSTR cmd, LPCSTR arg, int n_show);
#endif
#ifdef UTILS_BUILD_WITH_QT
bool getProcessCmdXp(unsigned long pid, QString& cmdLine);
QString getProcessCmd(unsigned long pid);
#endif

}
}
