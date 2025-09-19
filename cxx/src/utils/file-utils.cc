#include "file-utils.h"
#include "utils.h"

#ifdef WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x500
#endif
#endif



#ifndef WIN32
#include <config.h>
#include <pwd.h>
#include <uuid/uuid.h>
#endif

#ifndef WIN32
#include <unistd.h>
#include <dirent.h>
#endif
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <string.h>


#ifndef WIN32
#include <utime.h>
#endif

namespace {
#ifdef WIN32
int windows_error_to_errno(DWORD error)
{
    switch (error) {
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
        return ENOENT;
    case ERROR_ALREADY_EXISTS:
        return EEXIST;
    case ERROR_ACCESS_DENIED:
    case ERROR_SHARING_VIOLATION:
        return EACCES;
    case ERROR_DIR_NOT_EMPTY:
        return ENOTEMPTY;
    default:
        return 0;
    }
}
#endif // WIN32
}

NBC_NAMESPACE_BEGIN
NBC_UTILS_NAMESPACE_BEGIN

bool stat(const std::string &path, FileStat &st)
{
#ifdef WIN32
    auto wpath = win32_long_path(path);
    WIN32_FILE_ATTRIBUTE_DATA attrs;
    bool ret = true;

    if (!GetFileAttributesExW(wpath.c_str(), GetFileExInfoStandard, &attrs)) {
        ret = false;
        errno = windows_error_to_errno(GetLastError());
        goto out;
    }

    memset(&st, 0, sizeof(FileStat));

    if (attrs.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        st.st_mode = S_IFDIR;
    else
        st.st_mode = S_IFREG;

    st.st_atime = file_time_to_unix_time(&attrs.ftLastAccessTime);
    st.st_ctime = file_time_to_unix_time(&attrs.ftCreationTime);
    st.st_mtime = file_time_to_unix_time(&attrs.ftLastWriteTime);

    st.st_size = ((((__int64)attrs.nFileSizeHigh) << 32) + attrs.nFileSizeLow);

out:
    return ret;
#else
    return stat(path, st) == 0;
#endif
}

#ifdef WIN32
std::wstring win32_long_path(const std::string &path)
{
    // 使用 std::string 构造长路径前缀
    // UNC 路径 //server/share -> \\?\UNC\server\share
    std::string long_path = (path.rfind("//", 0) == 0)
        ? std::string("\\\\?\\UNC\\") + path.substr(2)
        : std::string("\\\\?\\") + path;

    // 将正斜杠统一替换为反斜杠
    for (auto& ch : long_path) {
        if (ch == '/')
            ch = '\\';
    }
    // 转 UTF-16（Windows 下本项目 utils 已提供此转换）
    return utils::local_codepage_to_utf16(long_path);
}

/* UNIX epoch expressed in Windows time, the unit is 100 nanoseconds.
 * See http://msdn.microsoft.com/en-us/library/ms724228
 */
#define UNIX_EPOCH 116444736000000000ULL

__time64_t file_time_to_unix_time(FILETIME *ftime)
{
    uint64_t win_time, unix_time;

    win_time = (uint64_t)ftime->dwLowDateTime + (((uint64_t)ftime->dwHighDateTime) << 32);
    unix_time = (win_time - UNIX_EPOCH) / 10000000;

    return (__time64_t)unix_time;
}
#endif // WIN32
NBC_UTILS_NAMESPACE_END
NBC_NAMESPACE_END

