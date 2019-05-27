#ifndef CACHE_CPP
#define CACHE_CPP

#include <unordered_map>
#include <sys/stat.h>
#include <shared_mutex>

using namespace std;


enum File_State {
    NORMAL,     // 正常
    NOTEXIST,   // 不存在
    OTHER       // 其他异常
};

struct file_cache {
    File_State state;
    char *file;
    size_t file_len;
    struct timespec mtime;
};


unordered_map<char *, struct file_cache *, hash_func, cmp> cache;

shared_mutex rw_lock;

enum Cache_State {
    HIT,        // 200 or 404
    MISS,       // 等待读取
    EXCEPTION   // 500
};

void add_to_NORMAL(int socket_fd) {
    auto &it = pack[socket_fd];
#ifdef DEBUG
    cout << "[debug] add cache: " << it.path << endl;
#endif
#ifdef DEBUG
    cout << "[debug] new cache entry mtime: " << f->mtime.tv_sec << ", " << f->mtime.tv_nsec << endl;
#endif
    rw_lock.lock();
    auto f = cache[it.path];
    if (f != nullptr) {
        free(f->file);
    } else {
        f = (struct file_cache *) malloc(sizeof(struct file_cache));
    }
    f->state = NORMAL;
    f->file = it.file;
    f->file_len = it.file_len;
    f->mtime.tv_sec = it.file_mtime.tv_sec;
    f->mtime.tv_nsec = it.file_mtime.tv_nsec;
    cache[it.path] = f;
    rw_lock.unlock();
#ifdef DEBUG
    for (auto &i:cache) {
        cout << "[debug] cache entry: " << i.first << endl;
    }
#endif
}

void change_to_NOTEXIST(struct file_cache *f, char *path) {
    if (f == nullptr) {
        f = (struct file_cache *) malloc(sizeof(struct file_cache));
        f->state = NOTEXIST;
        f->file = nullptr;
        rw_lock.lock();
        cache[path] = f;
        rw_lock.unlock();
    } else if (f->state != NOTEXIST) {
        f->state = NOTEXIST;
        free(f->file);
        f->file = nullptr;
    }
}

void change_to_OTHER(struct file_cache *f, char *path) {
    if (f == nullptr) {
        f = (struct file_cache *) malloc(sizeof(struct file_cache));
        f->state = OTHER;
        f->file = nullptr;
        rw_lock.lock();
        cache[path] = f;
        rw_lock.unlock();
    } else if (f->state != OTHER) {
        f->state = OTHER;
        free(f->file);
        f->file = nullptr;
    }
}

/**
 * 更新 cache 状态
 */
Cache_State update_cache(int socket_fd) {
    auto &it = pack[socket_fd];

    rw_lock.lock_shared();
    auto f = cache[it.path];
    rw_lock.unlock_shared();

    struct stat info{};
    // 获得文件修改时间等信息
    int ret = stat(it.path, &info);
    if (ret < 0) {
        if (errno == ENOENT) {
            // 文件不存在
            change_to_NOTEXIST(f, it.path);
            it.file = nullptr;
            it.file_len = 0;
            return HIT;
        } else {
            // 其他错误
            change_to_OTHER(f, it.path);
            it.file = nullptr;
            it.file_len = 0;
            return EXCEPTION;
        }
    } else {
        // 读到文件信息
        if (S_ISDIR(info.st_mode)) {
            // 是文件夹
            change_to_OTHER(f, it.path);
            it.file = nullptr;
            it.file_len = 0;
            return EXCEPTION;
        } else {
            // 正常文件，检查是否命中缓存，是否未修改
#ifdef DEBUG
            if (f == nullptr) {
                cout << "[debug] cache entry f is nullptr" << endl;
            } else {
                cout << "[debug] cache mtime: " << f->mtime.tv_sec << ", " << f->mtime.tv_nsec << endl;
                cout << "[debug] real mtime: " << info.st_mtim.tv_sec << ", " << info.st_mtim.tv_nsec << endl;
            }
#endif
            if (f != nullptr &&
                f->state == NORMAL &&
                f->mtime.tv_sec == info.st_mtim.tv_sec &&
                f->mtime.tv_nsec == info.st_mtim.tv_nsec
                    ) {
                // 命中缓存
                it.file = f->file;
                it.file_len = f->file_len;
#ifdef DEBUG
                cout << "[debug] cache hit: " << it.path << endl;
#endif
                return HIT;
            } else {
                // 不命中
                if (f != nullptr) {
                    // 擦除缓存
                    free(f->file);
                    free(f);
                    rw_lock.lock();
                    cache.erase(it.path);
                    rw_lock.unlock();
                }
                it.file = nullptr;
                it.file_len = info.st_size;     // 用于指示读文件
                it.file_mtime.tv_sec = info.st_mtim.tv_sec;
                it.file_mtime.tv_nsec = info.st_mtim.tv_nsec;
#ifdef DEBUG
                cout << "[debug] cache miss: " << it.path << endl;
#endif
                return MISS;
            }
        }
    }
}

#endif
