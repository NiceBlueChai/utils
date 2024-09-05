#pragma once

DWORD runShellAsAdministrator(LPCSTR cmd, LPCSTR arg, int n_show);

bool getProcessCmdXp(unsigned long pid, QString& cmdLine);
QString getProcessCmd(unsigned long pid);