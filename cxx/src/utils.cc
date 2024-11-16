#include "utils/utils.h"
#include <cerrno>
#include <iostream>
#include <sstream>

#ifdef _WIN32
#include "utils/utils-win.h"
#include <Windows.h>
#else
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#endif

NBC_NAMESPACE_BEGIN

namespace utils {
std::vector<std::string> split(const std::string &str, char delimiter) {
  std::vector<std::string> result;
  std::stringstream ss(str);
  std::string item;
  while (std::getline(ss, item, delimiter)) {
    result.push_back(item);
  }
  return result;
}

std::string join(const std::vector<std::string> &strs, char delimiter) {
  std::stringstream ss;
  for (size_t i = 0; i < strs.size(); i++) {
    ss << strs[i];
    if (i != strs.size() - 1) {
      ss << delimiter;
    }
  }
  return ss.str();
}

std::pair<std::string, std::string> rsplit1(const std::string &str,
                                            char delimiter) {
  auto pos = str.rfind(delimiter);
  if (pos == std::string::npos) {
    return std::make_pair(str, "");
  } else {
    return std::make_pair(str.substr(0, pos), str.substr(pos + 1));
  }
}

std::pair<std::string, std::string> split1(const std::string &str,
                                           char delimiter) {
  auto pos = str.find(delimiter);
  if (pos == std::string::npos) {
    return std::make_pair(str, "");
  } else {
    return std::make_pair(str.substr(0, pos), str.substr(pos + 1));
  }
}

bool is_running(const std::string &uid) {
#ifdef _WIN32
  auto _path = std::string("Global/") + uid;
#if defined(UNICODE) || defined(_UNICODE)
  auto path = local_codepage_to_utf16(_path);
#else
  auto &path = _path;
#endif
  auto mutex = CreateMutex(NULL, FALSE, path.c_str());
  if (mutex != NULL) {
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
      CloseHandle(mutex);
      return true;
    } else {
      return false;
    }
  }
  return true;
#else
  auto path = std::string("/tmp/") + uid;
  auto fd = open(path.c_str(), O_CREAT | O_RDWR,
                 (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH));
  if (fd < 0) {
    std::cerr << "open file failed: " << path << std::endl;
    exit(1);
  }
  extern int lockfile(int fd);
  if (lockfile(fd) < 0) {
    if (errno == EAGAIN || errno == EACCES) {
      close(fd);
      return true;
    }
    std::cerr << "can't lock " << path << ": " << strerror(errno) << std::endl;
    exit(1);
  }
  return false;
#endif
}

} // namespace utils

NBC_NAMESPACE_END
