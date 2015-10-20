#include "my_unix_commands.h"

using namespace std; 

int ok;
int err;

int my_lstat(const char *path, struct stat *buf) {
  return err;  
}

int my_readlink(const char *path, char *buf, size_t bufsiz) {
  return err;  
}

int my_close(ino_t fh) {
  return err;  
}

int my_mknode(const char *pathname, mode_t mode, dev_t dev) {
  return err;  
}

int my_mkdir(const char *pathname, mode_t mode) {
  return err;  
}

int my_unlink(const char *pathname) {
  return err;  
}

int my_rmdir(const char *pathname) {
  return err;  
}

int my_symlink(const char *oldpath, const char *newpath) {
  return err;  
}

int my_rename(const char *oldpath, const char *newpath) {
  return err;  
}

int my_link(const char *oldpath, const char *newpath) {
  return err;  
}

int my_chmod(const char *path, mode_t mode) {
  return err;  
}

int my_chown(const char *path, uid_t owner, gid_t group) {
  return err;  
}

int my_truncate(const char *path, off_t length) {
  return err;  
}

int my_utime(const char *filename, const struct utimbuf *times) {
  return err;  
}

int my_open(const char *pathname, int flags) {
  return err;  
}

int my_pread(int fh, void *buf, size_t count, off_t offset) {
  return err;  
}

int my_pwrite(int fh, void *buf, size_t count, off_t offset) {
  return err;  
}

int my_statvfs(const char *path, struct statvfs *buf) {
  return err;  
}

int my_fdatasync(ino_t fh) {
  return err;  
}

int my_fsync(ino_t fh) {
  return err;  
}

int my_lsetxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
  return err;  
}

int my_lgetxattr(const char *path, const char *name, const char *value, size_t size) {
  return err;  
}

int my_llistxattr(const char *path, char *list, size_t size) {
  return err;  
}

int my_lremovexattr(const char *path, const char *name) {
  return err;  
}

DIR *my_opendir(const char *name) {
  return err;  
}

struct dirent *my_readdir(DIR *dirp) {
  return err;  
}

int my_closedir(DIR *dirp) {
  return err;  
}

int my_access(const char *pathname, int mode) {
  return err;  
}

int my_creat(const char *path, mode_t mode) {
  return err;  
}

int my_ftruncate(int fd, off_t length) {
  return err;  
}

int my_fstat(int fd, struct stat *buf) {
  return err;  
}

char* my_realpath( const char* path, char* resolved_path) {
   assert( resolved_path == NULL );
   return err;
}
