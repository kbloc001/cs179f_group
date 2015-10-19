int my_lstat(const char *path, struct stat *buf);

int my_readlink(const char *path, char *buf, size_t bufsiz);

int my_open(const char *pathname, int flags, mode_t mode);

int my_close(int fd);

int my_mkfifo(const char *pathname, mode_t mode);

int my_mknode(const char *pathname, mode_t mode, dev_t dev);

int my_mkdir(const char *pathname, mode_t mode);

int my_unlink(const char *pathname);

int my_rmdir(const char *pathname);

int my_symlink(const char *oldpath, const char *newpath);

int my_rename(const char *oldpath, const char *newpath);

int my_link(const char *oldpath, const char *newpath);

int my_chmod(const char *path, mode_t mode);

int my_chown(const char *path, uid_t owner, gid_t group);

int my_truncate(const char *path, off_t length);

int my_utime(const char *filename, const struct utimbuf *times);

int my_open(const char *pathname, int flags);

int my_pread(int fd, void *buf, size_t count, off_t offset);

int my_pwrite(int fd, void *buf, size_t count, off_t offset);

int my_statvfs(const char *path, struct statvfs *buf);

int my_fdatasync(int fd);

int my_fsync(int fd);

int my_lsetxattr(const char *path, const char *name, const void *value, size_t size, int flags);

int my_lgetxattr(const char *path, const char *name, const void *value, size_t size);

int my_llistxattr(const char *path, char *list, size_t size);

int my_lremovexattr(const char *path, const char *name);

DIR *my_opendir(const char *name);

struct dirent *my_readdir(DIR *dirp);

int my_closedir(DIR *dirp);

int my_access(const char *pathname, int mode);

int my_creat(const char *path, mode_t mode); 

int my_ftruncate(int fd, off_t length);

int my_fstat(int fd, struct stat *buf);
