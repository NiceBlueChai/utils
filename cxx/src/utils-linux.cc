#include <fcntl.h>
#include <unistd.h>

#include <iostream>

namespace nbc {
namespace utils {
int lockfile(int fd) {
  struct flock fl;

  fl.l_type = F_WRLCK;
  fl.l_start = 0;
  fl.l_whence = SEEK_SET;
  fl.l_len = 0;
  return (fcntl(fd, F_SETLK, &fl));
}

pid_t lock_test(int fd, int type, off_t offset, int whence, off_t len) {
  struct flock lock;

  lock.l_type = type;     /* F_RDLCK or F_WRLCK */
  lock.l_start = offset;  /* byte offset, relative to l_whence */
  lock.l_whence = whence; /* SEEK_SET, SEEK_CUR, SEEK_END */
  lock.l_len = len;       /* #bytes (0 means to EOF) */

  if (fcntl(fd, F_GETLK, &lock) < 0) {
    std::cerr << "fcntl error" << std::endl;
    exit(1);
  }

  if (lock.l_type == F_UNLCK)
    return (0);        /* false, region isn't locked by another proc */
  return (lock.l_pid); /* true, return pid of lock owner */
}

} // namespace utils
} // namespace nbc
