#pragma once

DWORD runShellAsAdministrator(LPCSTR cmd, LPCSTR arg, int n_show);
#ifdef UTILS_BUILD_WITH_QT
bool getProcessCmdXp(unsigned long pid, QString& cmdLine);
QString getProcessCmd(unsigned long pid);
#endif