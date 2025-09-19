#pragma once

/**
 * @file file-utils.h
 * @brief 声明 nbc::utils 命名空间下的文件操作相关工具函数和类（不抛出异常的错误处理）。
 *
 * 本头文件用于声明与文件操作相关的工具函数或辅助类，便于在项目中统一管理文件相关功能。
 * 适用于 C++14 标准，支持 GTest 测试框架。
 */
#include "common.h"
#include <string>

#ifdef WIN32
#include <errno.h>
#include <windows.h>
#define mode_t int
 
#ifndef WEXITSTATUS
#define WEXITSTATUS(status) (((status) & 0xff00) >> 8)
#endif

#define FileStat struct __stat64
#else
#define FileStat struct stat
#endif

NBC_NAMESPACE_BEGIN
NBC_UTILS_NAMESPACE_BEGIN

/**
 * @brief 获取文件状态信息（不抛异常）
 * @param path 文件路径
 * @param out  输出的文件状态
 * @return 成功返回 true，失败返回 false（可通过 errno 获取错误码）
 */
bool stat(const std::string& path, FileStat& out);

/**
 * @brief 获取文件描述符对应的文件状态信息（不抛异常）
 * @param fd  文件描述符
 * @param out 输出的文件状态
 * @return 成功返回 true，失败返回 false（可通过 errno 获取错误码）
 */
bool fstat(int fd, FileStat& out);

/**
 * @brief 删除指定文件（不抛异常）
 * @param path 文件路径
 * @return 成功返回 true，失败返回 false（可通过 errno 获取错误码）
 */
bool unlink(const std::string& path);

/**
 * @brief 删除指定目录（不抛异常）
 * @param path 目录路径
 * @return 成功返回 true，失败返回 false（可通过 errno 获取错误码）
 */
bool rmdir(const std::string& path);

/**
 * @brief 创建目录（不抛异常）
 * @param path 目录路径
 * @param mode 权限模式
 * @return 成功返回 true，失败返回 false（可通过 errno 获取错误码）
 */
bool mkdir(const std::string& path, int mode = 0777);

/**
 * @brief 打开文件（不抛异常）
 * @param path  文件路径
 * @param flags 打开方式
 * @param mode  权限模式（在需要创建文件时使用）
 * @return 成功返回文件描述符(>=0)，失败返回 -1（可通过 errno 获取错误码）
 */
int open(const std::string& path, int flags, int mode = 0666);

/**
 * @brief 创建文件（不抛异常）
 * @param path 文件路径
 * @param mode 权限模式
 * @return 成功返回文件描述符(>=0)，失败返回 -1（可通过 errno 获取错误码）
 */
int create(const std::string& path, int mode = 0666);

/**
 * @brief 重命名文件或目录（不抛异常）
 * @param oldpath 原路径
 * @param newpath 新路径
 * @return 成功返回 true，失败返回 false（可通过 errno 获取错误码）
 */
bool rename(const std::string& oldpath, const std::string& newpath);

/**
 * @brief 判断文件或目录是否存在（不抛异常）
 * @param path 路径
 * @return 存在返回 true，不存在返回 false
 */
bool exists(const std::string& path);

#ifdef WIN32
std::wstring win32_long_path(const std::string& path);
__time64_t file_time_to_unix_time(FILETIME *ftime);
#endif // WIN32


NBC_UTILS_NAMESPACE_END
NBC_NAMESPACE_END
