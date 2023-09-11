#define FUSE_USE_VERSION 31
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define DFFS_USED_PATH  "/used"
#define DFFS_AVAIL_PATH "/avail"

static long fs_used = 0, fs_avail = 0;

static int dffs_getattr(const char* path, struct stat* stbuf)
{
    memset(stbuf, 0, sizeof(struct stat));

    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    } else if (strcmp(path, DFFS_USED_PATH) == 0 || strcmp(path, DFFS_AVAIL_PATH) == 0) {
        stbuf->st_mode = S_IFREG | 0666;
        stbuf->st_nlink = 1;
        stbuf->st_size = sizeof(long);
    } else {
        return -ENOENT;
    }

    return 0;
}

static int dffs_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi)
{
    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, "used", NULL, 0);
    filler(buf, "avail", NULL, 0);

    return 0;
}

static int dffs_open(const char* path, struct fuse_file_info* fi)
{
    if (strcmp(path, DFFS_USED_PATH) == 0 || strcmp(path, DFFS_AVAIL_PATH) == 0) {
        fi->flags |= O_RDWR;
        return 0;
    }

    return -ENOENT;
}

static int dffs_write(const char* path, const char* buf, size_t size, off_t offset, struct fuse_file_info* fi)
{
    long value;

    if (sscanf(buf, "%ld", &value) != 1) {
        return -EINVAL;
    }

    if (strcmp(path, DFFS_USED_PATH) == 0) {
        fs_used = value;
        printf("Set used to %ld\n", fs_used);
    } else if (strcmp(path, DFFS_AVAIL_PATH) == 0) {
        fs_avail = value;
        printf("Set avail to %ld\n", fs_avail);
    } else {
        return -ENOENT;
    }

    return size;
}

static int dffs_truncate(const char* path, off_t size)
{
    return 0;
}

static int dffs_statfs(const char* path, struct statvfs* stbuf)
{
    memset(stbuf, 0, sizeof(struct statvfs));

    stbuf->f_bsize = 1;
    stbuf->f_frsize = 1;
    stbuf->f_blocks = fs_avail + fs_used;
    stbuf->f_bfree = fs_avail;
    stbuf->f_bavail = stbuf->f_bfree;

    return 0;
}

static struct fuse_operations operations = {
    .getattr = dffs_getattr,
    .readdir = dffs_readdir,
    .open = dffs_open,
    .write = dffs_write,
    .statfs = dffs_statfs,
    .truncate = dffs_truncate,
};

int main(int argc, char* argv[])
{
    return fuse_main(argc, argv, &operations, NULL);
}
