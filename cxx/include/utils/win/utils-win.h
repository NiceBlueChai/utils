#pragma once



#include <string>
#include <windows.h>

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
namespace win {
// a list for windows versions https://msdn.microsoft.com/en-us/library/windows/desktop/ms724833%28v=vs.85%29.aspx
// Windows Relaese        major    minor   patch(SP)
// windows 10:            10,      0,      ?
// windows 8.1:           6,       3,      ?
// windows 2012 R2:       6,       3,      ?
// windows 8:             6,       2,      ?
// windows 2012:          6,       2,      ?
// windows 7:             6,       1,      ?
// windows 2008 R2:       6,       1,      ?
// windows Vista:         6,       0,      ?
// windows 2008:          6,       0,      ?
// windows 2003 R2:       5,       2,      ?
// windows 2003:          5,       2,      ?
// windows XP x64:        5,       2,      ?
// windows XP:            5,       1,      ?
// windows 2000:          5,       0,      ?
void getSystemVersion(unsigned *major, unsigned *minor, unsigned *patch);
bool isAtLeastSystemVersion(unsigned major, unsigned minor, unsigned patch);

bool isWindowsVistaOrGreater();
bool isWindows7OrGreater();
bool isWindows8OrGreater();
bool isWindows8Point1OrGreater();
bool isWindows10OrHigher();
bool fixQtHDPINonIntegerScaling();
std::string getLocalPipeName(const char *pipeName);
} // namespace win
} // namespace utils
} // namespace nbc
