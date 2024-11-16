#pragma once

#include "common.h"


NBC_NAMESPACE_BEGIN
namespace utils
{
int lockfile(int fd);
pid_t lock_test(int fd, int type, off_t offset, int whence, off_t len) 

}

NBC_NAMESPACE_END