#include <windows.h>

// Qt's QProcess function cannot invoke programs that require administrator privileges,
// so we need windows api funtion to invoke the program that require adminstrator privileges.
DWORD runShellAsAdministrator(LPCSTR cmd, LPCSTR arg, int n_show)
{
  SHELLEXECUTEINFO shell_exec_info = {0};
  shell_exec_info.cbSize = sizeof(SHELLEXECUTEINFO);
  shell_exec_info.fMask = SEE_MASK_NOCLOSEPROCESS;
  shell_exec_info.hwnd = NULL;
  shell_exec_info.lpVerb = "runas";
  shell_exec_info.lpFile = cmd;
  shell_exec_info.lpParameters = arg;
  shell_exec_info.lpDirectory = NULL;
  shell_exec_info.nShow = n_show;
  shell_exec_info.hInstApp = NULL;

  BOOL ret = ShellExecuteEx(&shell_exec_info);
  WaitForSingleObject(shell_exec_info.hProcess, INFINITE);

  DWORD exit_code=0;
  GetExitCodeProcess(shell_exec_info.hProcess, &exit_code);
  CloseHandle(shell_exec_info.hProcess);
  return exit_code;
}

// 只适用于32位系统
//winxp，win7，win8 32位系统测试有效
bool getProcessCmdXp(unsigned long pid, QString& cmdLine) {
  HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
  if (hProcess == INVALID_HANDLE_VALUE || hProcess == NULL){
    return false;
  }

  BOOL bRet = FALSE;
  DWORD dwPos = 0;
  LPBYTE lpAddr = NULL;
  DWORD dwRetLen = 0;
  CHAR cmdBuffer[MAX_PATH*2] = {0};

  lpAddr = (LPBYTE)GetCommandLine;

 lpAddr = (LPBYTE)GetCommandLine;
Win7:
	if(lpAddr[dwPos] == 0xeb && lpAddr[dwPos + 1] == 0x05) {
		dwPos += 2;
		dwPos += 5;
Win8:
		if(lpAddr[dwPos] == 0xff && lpAddr[dwPos + 1] == 0x25) {
			dwPos += 2;
			lpAddr = *(LPBYTE*)(lpAddr + dwPos);
 
			dwPos = 0;
			lpAddr = *(LPBYTE*)lpAddr;
WinXp:
			if(lpAddr[dwPos] == 0xa1) {
				dwPos += 1;
				lpAddr = *(LPBYTE*)(lpAddr + dwPos);
				bRet = ReadProcessMemory(hProcess, lpAddr, &lpAddr, sizeof(LPBYTE),
					&dwRetLen);
				if(bRet) {
					bRet = ReadProcessMemory(hProcess, lpAddr, cmdBuffer, sizeof(cmdBuffer),
						&dwRetLen);

            if(bRet) {
              auto codec = QTextCodec::codecForName("GBK");
              cmdLine = codec->toUnicode(cmdBuffer);
            }
				}
			}
		} else {			
			goto WinXp;
		}
	} else {
		goto Win8;
	}
 
	return bRet;
}



#include "winternl.h"
typedef NTSTATUS  (WINAPI *NtQueryInformationProcessFake)(HANDLE, DWORD, PVOID, ULONG, PULONG);
 
NtQueryInformationProcessFake ntQ = NULL;
QString getProcessCmd(unsigned long pid) {
  QString cmdStr;
  HANDLE hproc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid );
	if (INVALID_HANDLE_VALUE != hproc){
		HANDLE hnewdup = NULL;
		PEB peb;
		RTL_USER_PROCESS_PARAMETERS upps;
		WCHAR buffer[MAX_PATH] = {NULL};
		HMODULE hm = LoadLibrary(_T("Ntdll.dll"));
		ntQ = (NtQueryInformationProcessFake)GetProcAddress(hm, "NtQueryInformationProcess");
		if ( DuplicateHandle(GetCurrentProcess(), hproc, GetCurrentProcess(), &hnewdup, 0, FALSE, DUPLICATE_SAME_ACCESS) ) {
			PROCESS_BASIC_INFORMATION pbi;
			NTSTATUS isok = ntQ(hnewdup, 0/*ProcessBasicInformation*/, (PVOID)&pbi, sizeof(PROCESS_BASIC_INFORMATION), 0);        
			if (BCRYPT_SUCCESS(isok)) {
				if ( ReadProcessMemory(hnewdup, pbi.PebBaseAddress, &peb, sizeof(PEB), 0) )
					if ( ReadProcessMemory(hnewdup, peb.ProcessParameters, &upps, sizeof(RTL_USER_PROCESS_PARAMETERS), 0) ) {
						WCHAR *buffer = new WCHAR[upps.CommandLine.Length + 1];
						ZeroMemory(buffer, (upps.CommandLine.Length + 1) * sizeof(WCHAR));
						ReadProcessMemory(hnewdup, upps.CommandLine.Buffer, buffer, upps.CommandLine.Length, 0);
						cmdStr = QString::fromWCharArray(buffer);
            delete buffer;
					}
			}
			CloseHandle(hnewdup);
		}
 
		CloseHandle(hproc);
	}
  return cmdStr;
 }
 

 
 
