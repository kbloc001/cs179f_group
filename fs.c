#include "params.h"
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <libgen.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#ifdef HAVE_SYS_XATTR_H
#include <sys/xattr.h>
#endif

#include "log.h"

static int _error(char *str){}


static void _fullpath(char fpath[PATH_MAX], const char *path){}

int _getattr(const char *path, struct stat *statbuf){}

int _readlink(const char *path, char *link, size_t size){}

int _mknod(const char *path, mode_t mode, dev_t dev){}


int _mkdir(const char *path, mode_t mode){}


int _unlink(const char *path){}


int _rmdir(const char *path){}

int _symlink(const char *path, const char *link){}

int _rename(const char *path, const char *newpath){}

int _link(const char *path, const char *newpath){}

int _chmod(const char *path, mode_t mode){}


int _chown(const char *path, uid_t uid, gid_t gid){}


int _truncate(const char *path, off_t newsize){}


int _utime(const char *path, struct utimbuf *ubuf){}

// Part 1
int _open(const char *path, struct fuse_file_info *fi){}


int _read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi){}

int _write(const char *path, const char *buf, size_t size, off_t offset,
         struct fuse_file_info *fi){}


int _statfs(const char *path, struct statvfs *statv){}

int _flush(const char *path, struct fuse_file_info *fi){}

int _release(const char *path, struct fuse_file_info *fi){}


int _fsync(const char *path, int datasync, struct fuse_file_info *fi){}

#ifdef HAVE_SYS_XATTR_H
int _setxattr(const char *path, const char *name, const char *value, size_t size, int flags){}

int _getxattr(const char *path, const char *name, char *value, size_t size){}


int _listxattr(const char *path, char *list, size_t size){}


int _removexattr(const char *path, const char *name){}
#endif

int _opendir(const char *path, struct fuse_file_info *fi){}

int _readdir(const char *path, void *buf, off_t offset,
           struct fuse_file_info *fi){} //fuse_fill_dir_t filler

int _releasedir(const char *path, struct fuse_file_info *fi){}

int _fsyncdir(const char *path, int datasync, struct fuse_file_info *fi){}


void *_init(struct fuse_conn_info *conn){}


void _destroy(void *userdata){}


int _access(const char *path, int mask){}



int _create(const char *path, mode_t mode, struct fuse_file_info *fi){}



int _ftruncate(const char *path, off_t offset, struct fuse_file_info *fi){}


int _fgetattr(const char *path, struct stat *statbuf, struct fuse_file_info *fi){}

struct fuse_operations _oper = {
    .getattr = _getattr,
    .readlink = _readlink,
    .getdir = NULL,
    .mknod = _mknod,
    .mkdir = _mkdir,
    .unlink = _unlink,
    .rmdir = _rmdir,
    .symlink = _symlink,
    .rename = _rename,
    .link = _link,
    .chmod = _chmod,
    .chown = _chown,
    .truncate = _truncate,
    .utime = _utime,
    .open = _open,
    .read = _read,
    .write = _write,
    .statfs = _statfs,
    .flush = _flush,
    .release = _release,
    .fsync = _fsync,

#ifdef HAVE_SYS_XATTR_H
    .setxattr = _setxattr,
    .getxattr = _getxattr,
    .listxattr = _listxattr,
    .removexattr = _removexattr,
# endif
   
    .opendir = _opendir,
       .readdir = _readdir,
    .releaseddir = _releaseddir,
    .fsyncdir = _fsyncdir,
    .init = _init,
    .destroy = _destroy,
    .access = _access,
    .create = _create,
    .ftruncate = _ftruncate,
    .fgetattr = _fgetattr
};

void bb_usage(){}

int main(int argc, char *argv[]){}
