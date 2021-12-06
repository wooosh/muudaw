#include <libmuu/fs.h>
#include <libmuu/mman.h>
#include <libmuu/util.h>
#include <fcntl.h>
#include <unistd.h>

void FSLoadFile(struct MemManager *mman, struct MemRegion *region, char *path) {
  int fd = open(path, O_RDONLY);
  assert_fatal(fd != -1, "%s", MSG_ERRNO);

  // allocate enough space for the file
  off_t len = lseek(fd, 0, SEEK_END);
  assert_fatal(len != -1, "%s", MSG_ERRNO);
  MemManagerResizeRegion(mman, region, len);
  
  off_t seek_err = lseek(fd, 0, SEEK_SET);
  assert_fatal(seek_err != -1, "%s", MSG_ERRNO);

  ssize_t read_len = read(fd, (char*) region->start, len);
  assert_fatal(read_len != -1, MSG_ERRNO);
  assert_fatal(read_len == len, "incomplete read");

  int err = close(fd);
  assert_fatal(err != -1, "%s", MSG_ERRNO);
}
