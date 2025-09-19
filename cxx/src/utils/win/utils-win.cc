#include <Windows.h>
#include <TChar.h>
#include "utils/utils.h"
#include "utils/common.h"

NBC_NAMESPACE_BEGIN

namespace utils {


std::wstring local_codepage_to_utf16(const std::string& input) {
    size_t       need = MultiByteToWideChar(CP_ACP, 0, input.c_str(), input.size(), 0, 0);
    wchar_t     *wstr = static_cast<wchar_t *>(malloc(need + sizeof(wchar_t)));
    size_t       used = MultiByteToWideChar(CP_ACP, 0, input.c_str(), input.size(), wstr, need);
    std::wstring result(wstr, used);
    free(wstr);
    return result;
}

std::string utf16_to_local_codepage(wchar_t * data, size_t len) {
    BOOL ignore;

    size_t      need = WideCharToMultiByte(CP_ACP, 0, data, len, 0, 0, "?", &ignore);
    char       *cstr = static_cast<char *>(malloc(need + 1));
    size_t      used = WideCharToMultiByte(CP_ACP, 0, data, len, cstr, need, "?", &ignore);
    std::string result(cstr, used);
    free(cstr);
    return result;
}

// Qt's QProcess function cannot invoke programs that require administrator
// privileges, so we need windows api funtion to invoke the program that require
// adminstrator privileges.
#if defined(UNICODE) || defined(_UNICODE)
DWORD runShellAsAdministrator(LPCWSTR cmd, LPCWSTR arg, int n_show) {
#else
DWORD runShellAsAdministrator(LPCSTR cmd, LPCSTR arg, int n_show) {
#endif
  SHELLEXECUTEINFO shell_exec_info = {0};
  shell_exec_info.cbSize = sizeof(SHELLEXECUTEINFO);
  shell_exec_info.fMask = SEE_MASK_NOCLOSEPROCESS;
  shell_exec_info.hwnd = NULL;
  shell_exec_info.lpVerb = _TEXT("runas");
  shell_exec_info.lpFile = cmd;
  shell_exec_info.lpParameters = arg;
  shell_exec_info.lpDirectory = NULL;
  shell_exec_info.nShow = n_show;
  shell_exec_info.hInstApp = NULL;

  BOOL ret = ShellExecuteEx(&shell_exec_info);
  WaitForSingleObject(shell_exec_info.hProcess, INFINITE);

  DWORD exit_code = 0;
  GetExitCodeProcess(shell_exec_info.hProcess, &exit_code);
  CloseHandle(shell_exec_info.hProcess);
  return exit_code;
}
#ifdef UTILS_BUILD_WITH_QT
// 只适用于32位系统
// winxp，win7，win8 32位系统测试有效
bool getProcessCmdXp(unsigned long pid, QString &cmdLine) {
  HANDLE hProcess =
      OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
  if (hProcess == INVALID_HANDLE_VALUE || hProcess == NULL) {
    return false;
  }

  BOOL bRet = FALSE;
  DWORD dwPos = 0;
  LPBYTE lpAddr = NULL;
  DWORD dwRetLen = 0;
  CHAR cmdBuffer[MAX_PATH * 2] = {0};

  lpAddr = (LPBYTE)GetCommandLine;

  lpAddr = (LPBYTE)GetCommandLine;
Win7:
  if (lpAddr[dwPos] == 0xeb && lpAddr[dwPos + 1] == 0x05) {
    dwPos += 2;
    dwPos += 5;
  Win8:
    if (lpAddr[dwPos] == 0xff && lpAddr[dwPos + 1] == 0x25) {
      dwPos += 2;
      lpAddr = *(LPBYTE *)(lpAddr + dwPos);

      dwPos = 0;
      lpAddr = *(LPBYTE *)lpAddr;
    WinXp:
      if (lpAddr[dwPos] == 0xa1) {
        dwPos += 1;
        lpAddr = *(LPBYTE *)(lpAddr + dwPos);
        bRet = ReadProcessMemory(hProcess, lpAddr, &lpAddr, sizeof(LPBYTE),
                                 &dwRetLen);
        if (bRet) {
          bRet = ReadProcessMemory(hProcess, lpAddr, cmdBuffer,
                                   sizeof(cmdBuffer), &dwRetLen);

          if (bRet) {
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
typedef NTSTATUS(WINAPI *NtQueryInformationProcessFake)(HANDLE, DWORD, PVOID,
                                                        ULONG, PULONG);

NtQueryInformationProcessFake ntQ = NULL;
QString getProcessCmd(unsigned long pid) {
  QString cmdStr;
  HANDLE hproc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
  if (INVALID_HANDLE_VALUE != hproc) {
    HANDLE hnewdup = NULL;
    PEB peb;
    RTL_USER_PROCESS_PARAMETERS upps;
    WCHAR buffer[MAX_PATH] = {NULL};
    HMODULE hm = LoadLibrary(_T("Ntdll.dll"));
    ntQ = (NtQueryInformationProcessFake)GetProcAddress(
        hm, "NtQueryInformationProcess");
    if (DuplicateHandle(GetCurrentProcess(), hproc, GetCurrentProcess(),
                        &hnewdup, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
      PROCESS_BASIC_INFORMATION pbi;
      NTSTATUS isok = ntQ(hnewdup, 0 /*ProcessBasicInformation*/, (PVOID)&pbi,
                          sizeof(PROCESS_BASIC_INFORMATION), 0);
      if (BCRYPT_SUCCESS(isok)) {
        if (ReadProcessMemory(hnewdup, pbi.PebBaseAddress, &peb, sizeof(PEB),
                              0))
          if (ReadProcessMemory(hnewdup, peb.ProcessParameters, &upps,
                                sizeof(RTL_USER_PROCESS_PARAMETERS), 0)) {
            WCHAR *buffer = new WCHAR[upps.CommandLine.Length + 1];
            ZeroMemory(buffer, (upps.CommandLine.Length + 1) * sizeof(WCHAR));
            ReadProcessMemory(hnewdup, upps.CommandLine.Buffer, buffer,
                              upps.CommandLine.Length, 0);
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
#endif

namespace win {

namespace {
OSVERSIONINFOEX osver; // static variable, all zero
bool osver_failure = false;

// From http://stackoverflow.com/a/36909293/1467959 and http://yamatyuu.net/computer/program/vc2013/rtlgetversion/index.html
typedef void(WINAPI *RtlGetVersion_FUNC)(OSVERSIONINFOEXW *);
BOOL CustomGetVersion(OSVERSIONINFOEX *os)
{
    HMODULE hMod;
    RtlGetVersion_FUNC func;
#ifdef UNICODE
    OSVERSIONINFOEXW *osw = os;
#else
    OSVERSIONINFOEXW o;
    OSVERSIONINFOEXW *osw = &o;
#endif

    hMod = LoadLibrary(TEXT("ntdll.dll"));
    if (hMod) {
        func = (RtlGetVersion_FUNC)GetProcAddress(hMod, "RtlGetVersion");
        if (func == 0) {
            FreeLibrary(hMod);
            return FALSE;
        }
        ZeroMemory(osw, sizeof(*osw));
        osw->dwOSVersionInfoSize = sizeof(*osw);
        func(osw);
#ifndef UNICODE
        os->dwBuildNumber = osw->dwBuildNumber;
        os->dwMajorVersion = osw->dwMajorVersion;
        os->dwMinorVersion = osw->dwMinorVersion;
        os->dwPlatformId = osw->dwPlatformId;
        os->dwOSVersionInfoSize = sizeof(*os);
        DWORD sz = sizeof(os->szCSDVersion);
        WCHAR *src = osw->szCSDVersion;
        unsigned char *dtc = (unsigned char *)os->szCSDVersion;
        while (*src)
            *dtc++ = (unsigned char)*src++;
        *dtc = '\0';
#endif

    } else
        return FALSE;
    FreeLibrary(hMod);
    return TRUE;
}


inline bool isInitializedSystemVersion() { return osver.dwOSVersionInfoSize != 0; }
inline void initializeSystemVersion() {
    if (isInitializedSystemVersion()) {
        return;
    }
    osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    // according to the document,
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms724451%28v=vs.85%29.aspx
    // this API will be unavailable once windows 10 is out
    if (!CustomGetVersion(&osver)) {
        qWarning("failed to get OS vesion.");
        osver_failure = true;
    }
}

inline bool _isAtLeastSystemVersion(unsigned major, unsigned minor, unsigned patch)
{
    initializeSystemVersion();
    if (osver_failure) {
        return false;
    }
#define OSVER_TO_NUM(major, minor, patch) ((major << 20) + (minor << 10) + (patch))
#define OSVER_SYS(ver) OSVER_TO_NUM(ver.dwMajorVersion, ver.dwMinorVersion, ver.wServicePackMajor)
    if (OSVER_SYS(osver) < OSVER_TO_NUM(major, minor, patch)) {
        return false;
    }
#undef OSVER_SYS
#undef OSVER_TO_NUM
    return true;
}

// compile statically
template<unsigned major, unsigned minor, unsigned patch>
inline bool isAtLeastSystemVersion()
{
    return _isAtLeastSystemVersion(major, minor, patch);
}
} // anonymous namesapce

void getSystemVersion(unsigned *major, unsigned *minor, unsigned *patch)
{
    initializeSystemVersion();
    // default to XP
    if (osver_failure) {
        *major = 5;
        *minor = 1;
        *patch = 0;
    }
    *major = osver.dwMajorVersion;
    *minor = osver.dwMinorVersion;
    *patch = osver.wServicePackMajor;
}

bool isAtLeastSystemVersion(unsigned major, unsigned minor, unsigned patch)
{
    return _isAtLeastSystemVersion(major, minor, patch);
}

bool isWindowsVistaOrHigher()
{
    return isAtLeastSystemVersion<6, 0, 0>();
}

bool isWindows7OrHigher()
{
    return isAtLeastSystemVersion<6, 1, 0>();
}

bool isWindows8OrHigher()
{
    return isAtLeastSystemVersion<6, 2, 0>();
}

bool isWindows8Point1OrHigher()
{
    return isAtLeastSystemVersion<6, 3, 0>();
}

bool isWindows10OrHigher()
{
    return isAtLeastSystemVersion<10, 0, 0>();
}

typedef HRESULT (WINAPI *GetDpiForMonitor)(HMONITOR,int,UINT *,UINT *);
typedef BOOL (WINAPI *SetProcessDPIAware)();
GetDpiForMonitor getDpiForMonitor;
SetProcessDPIAware setProcessDPIAware;
typedef QPair<qreal, qreal> QDpi;

static inline QDpi monitorDPI(HMONITOR hMonitor)
{
    UINT dpiX;
    UINT dpiY;
    if (SUCCEEDED(getDpiForMonitor(hMonitor, 0, &dpiX, &dpiY)))
        return QDpi(dpiX, dpiY);
    return QDpi(0, 0);
}

static inline QDpi deviceDPI(HDC hdc)
{
    return QDpi(GetDeviceCaps(hdc, LOGPIXELSX), GetDeviceCaps(hdc, LOGPIXELSY));
}

static bool monitorData(HMONITOR hMonitor, QDpi *dpi_out)
{
    MONITORINFOEX info;
    memset(&info, 0, sizeof(MONITORINFOEX));
    info.cbSize = sizeof(MONITORINFOEX);
    if (GetMonitorInfo(hMonitor, &info) == FALSE)
        return false;

    if (QString::fromLocal8Bit(info.szDevice) == QLatin1String("WinDisc")) {
        return false;
    } else {
        QDpi dpi = monitorDPI(hMonitor);
        if (dpi.first) {
            *dpi_out = dpi;
            return true;
        } else {
            HDC hdc = CreateDC(info.szDevice, NULL, NULL, NULL);
            if (hdc) {
                *dpi_out = deviceDPI(hdc);
                DeleteDC(hdc);
                return true;
            }
        }
    }
    return false;
}

bool monitorEnumCallback(HMONITOR hMonitor, HDC hdc, LPRECT rect, LPARAM p)
{
    QDpi *data = (QDpi *)p;
    if (monitorData(hMonitor, data)) {
        // printf ("dpi = %d %d\n", (int)data->first, (int)data->second);
        return false;
    }
    return true;
}

static bool readDPI(QDpi *dpi)
{
    EnumDisplayMonitors(0, 0, (MONITORENUMPROC)monitorEnumCallback, (LPARAM)dpi);
    return dpi->first != 0;
}


// QT's HDPI doesn't support non-integer scale factors, but QT_SCALE_FACTOR
// environment variable could work with them. So here we calculate the scaling
// factor (by reading the screen DPI), and update the value QT_SCALE_FACTOR with
// it.
//
// NOTE: The code below only supports single monitor. For multiple monitors we
// need to detect the dpi of each monitor and set QT_AUTO_SCREEN_SCALE_FACTOR
// accordingly. We may do that in the future.
bool fixQtHDPINonIntegerScaling()
{
    // Only do this on win8/win10
    if (!isWindows8OrHigher()) {
        return false;
    }
    // Don't overwrite the user sepcified scaling factors
    if (!qgetenv("QT_SCALE_FACTOR").isEmpty() || !qgetenv("QT_SCREEN_SCALE_FACTORS").isEmpty()) {
        return true;
    }
    // Don't overwrite the user sepcified multi-screen scaling factors
    if (!qgetenv("QT_AUTO_SCREEN_SCALE_FACTOR").isEmpty()) {
        return true;
    }

    // GetDpiForMonitor and SetProcessDPIAware are only available on win8/win10.
    //
    // See:
    //   - GetDpiForMonitor https://msdn.microsoft.com/en-us/library/windows/desktop/dn280510(v=vs.85).aspx
    //   - SetProcessDPIAware https://msdn.microsoft.com/en-us/library/windows/desktop/ms633543(v=vs.85).aspx
    QLibrary shcore_dll(QString("SHCore"));
    getDpiForMonitor = (GetDpiForMonitor)shcore_dll.resolve("GetDpiForMonitor");
    if (getDpiForMonitor == nullptr) {
        return false;
    }

    QLibrary user32_dll(QString("user32"));
    setProcessDPIAware = (SetProcessDPIAware)user32_dll.resolve("SetProcessDPIAware");
    if (setProcessDPIAware == nullptr) {
        return false;
    }

    // Turn off system scaling, otherwise we'll always see a 96 DPI virtual screen.
    if (!setProcessDPIAware()) {
        return false;
    }

    QDpi dpi;
    if (!readDPI(&dpi)) {
        return false;
    }

    if (dpi.first <= 96) {
        return false;
    }

    // See the "DPI and the Desktop Scaling Factor" https://msdn.microsoft.com/en-us/library/windows/desktop/dn469266(v=vs.85).aspx#dpi_and_the_desktop_scaling_factor
    // Specifically, MSDN says:
    //     96 DPI = 100% scaling
    //     120 DPI = 125% scaling
    //     144 DPI = 150% scaling
    //     192 DPI = 200% scaling
    double scaling_factor = ((double)(dpi.first)) / 96.0;
    QString factor = QString::number(scaling_factor);

    // Use QT_SCREEN_SCALE_FACTORS instead of QT_SCALE_FACTOR. The latter would
    // scale the font, which has already been scaled by the system.
    //
    // See also http://lists.qt-project.org/pipermail/interest/2015-October/019242.html
    g_setenv("QT_SCREEN_SCALE_FACTORS", factor.toUtf8().data(), 1);
    // printf("set QT_SCALE_FACTOR to %s\n", factor.toUtf8().data());
    return true;
}

char *b64encode(const char *input)
{
    char buf[32767] = {0};
    DWORD retlen = 32767;
    CryptBinaryToString((BYTE*) input, strlen(input), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, buf, &retlen);
    return strdup(buf);
}

std::string getLocalPipeName(const char *pipe_name)
{
#if defined(_MSC_VER)
    const DWORD buf_char_count = 32767;
    DWORD buf_char_count_size = buf_char_count;
#else
    DWORD buf_char_count = 32767;
#endif
    char user_name_buf[buf_char_count];

#if defined(_MSC_VER)
    if (GetUserName(user_name_buf, &buf_char_count_size) == 0) {
#else
    if (GetUserName(user_name_buf, &buf_char_count) == 0) {
#endif
        qWarning("Failed to get user name, GLE=%lu\n", GetLastError());
        return pipe_name;
    } else {
        std::string ret(pipe_name);
        char *encoded = b64encode(user_name_buf);
        ret += encoded;
        free(encoded);
        return ret;
    }
}



} // namespace win
}

NBC_NAMESPACE_END
