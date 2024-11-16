#include "utils/utils-linux.h"
#include "utils/utils.h"
#include <fcntl.h>
#include <unistd.h>

#include <iostream>
#include <vector>
#include <string>
#include <locale>
#include <iconv.h>

NBC_NAMESPACE_BEGIN
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



std::string local_codepage_to_utf16(const std::string& input) {
    iconv_t conv = iconv_open("UTF-16LE", "UTF-8");
    if (conv == (iconv_t)-1) {
        perror("iconv_open");
        return "";
    }

    size_t            in_bytes_left  = input.size();
    size_t            out_bytes_left = in_bytes_left * 4 + 2;
    std::vector<char> outbuf(out_bytes_left, 0);

    char *in_buf  = const_cast<char *>(input.data());
    char *out_ptr = outbuf.data();

    if (iconv(conv, &in_buf, &in_bytes_left, &out_ptr, &out_bytes_left) == (size_t)-1) {
        perror("iconv");
        iconv_close(conv);
        return "";
    }

    *out_ptr++ = 0;
    *out_ptr++ = 0;

    iconv_close(conv);
    std::string utf16_str(outbuf.data(), out_ptr - outbuf.data());
    return utf16_str;
}

std::string utf16_to_local_codepage(char16_t *data, size_t len) {
    std::string utf16_str((char *)data, len * sizeof(char16_t));

    iconv_t conv = iconv_open("UTF-8", "UTF-16LE");
    if (conv == (iconv_t)-1) {
        perror("iconv_open");
        return "";
    }

    size_t            in_bytes_left  = utf16_str.size();
    size_t            out_bytes_left = in_bytes_left * 4;
    std::vector<char> outbuf(out_bytes_left, 0);

    char *in_buf  = const_cast<char *>(utf16_str.data());
    char *out_ptr = outbuf.data();

    if (iconv(conv, &in_buf, &in_bytes_left, &out_ptr, &out_bytes_left) == (size_t)-1) {
        perror("iconv");
        iconv_close(conv);
        return "";
    }

    iconv_close(conv);
    return std::string(outbuf.data());
}

} // namespace utils
NBC_NAMESPACE_END
