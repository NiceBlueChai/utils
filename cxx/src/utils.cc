#include "utils/utils.h"
#include <sstream>

#ifdef _WIN32
#include <Windows.h>
#else
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

std::string join(const std::vector<std::string>& strs, char delimiter) {
    std::stringstream ss;
    for (size_t i = 0; i < strs.size(); i++) {
        ss << strs[i];
        if (i != strs.size() - 1) {
            ss << delimiter;
        }
    }
    return ss.str();
}

std::pair<std::string, std::string> rsplit1(const std::string& str, char delimiter) {
    auto pos = str.rfind(delimiter);
    if (pos == std::string::npos) {
        return std::make_pair(str, "");
    } else {
        return std::make_pair(str.substr(0, pos), str.substr(pos+1));
    }
}

std::pair<std::string,std::string> split1(const std::string& str, char delimiter) {
    auto pos = str.find(delimiter);
    if (pos == std::string::npos) {
        return std::make_pair(str, "");
    } else {
        return std::make_pair(str.substr(0, pos), str.substr(pos+1));
    }
}

bool is_running(const std::string& uid) {
#ifdef _WIN32
    auto path = std::string("Global/") + uid;
    auto mutex = CreateMutexA(NULL, FALSE, path.c_str());
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
#endif
}

} // namespace utils

NBC_NAMESPACE_END