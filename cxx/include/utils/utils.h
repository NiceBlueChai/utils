#pragma once

#include "common.h"

#include <vector>
#include <string>
#include <utility>

NBC_NAMESPACE_BEGIN

namespace utils {
/* string */
// 分割字符串 返回分割后的字符串数组
NBC_EXPORT std::vector<std::string> split(const std::string& str, char delimiter);
// 将字符串数组用 delimiter 连接成一个字符串 返回连接后的字符串
NBC_EXPORT std::string join(const std::vector<std::string>& strs, char delimiter);
// 从右往左截取字符串，直到遇到 delimiter 为止 返回截取的字符串和剩余的字符串
NBC_EXPORT std::pair<std::string, std::string> rsplit1(const std::string& str, char delimiter);
// 从左往右截取字符串，直到遇到 delimiter 为止 返回截取的字符串和剩余的字符串
NBC_EXPORT std::pair<std::string,std::string> split1(const std::string& str, char delimiter);

NBC_EXPORT bool is_running(const std::string& uid);


#ifdef __linux
std::string utf16_to_local_codepage(char16_t *data, size_t len);
std::string local_codepage_to_utf16(const std::string& input);
#else
std::wstring local_codepage_to_utf16(const std::string& input);
std::string  utf16_to_local_codepage(wchar_t *data, size_t len);
#endif
}

NBC_NAMESPACE_END
