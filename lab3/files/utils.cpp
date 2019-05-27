#ifndef UTILS_CPP
#define UTILS_CPP

#include <limits.h>

#include "epoll.h"
#include "const.h"

using namespace std;

enum Resp_Type {
    RESP_200,
    RESP_404,
    RESP_500
};

// 静态数据分配
struct {
    int socket_fd;
    // receive
    char *req;
    size_t req_len;
    size_t recvd_len;
    size_t max_req_len;
    // parse
    char *path;
    size_t path_len;
    char *real_path;
    size_t real_path_len;
    // read
    char *file;
    size_t file_len;
    struct timespec file_mtime;
    int file_fd;
    size_t read_len;
    struct aiocb file_cb;   // 异步 IO
    // send
    Resp_Type type;
    size_t sent_len;
    char *head;
    size_t head_len;
} pack[PACK_NUM];

char *cwd;
size_t cwd_len;

void init_utils() {
    cwd = get_current_dir_name();
    cwd_len = strlen(cwd);
#ifdef DEBUG
    cout << "[debug] cwd: " << cwd << endl;
#endif
}

bool is_escape(int socket_fd) {
    auto &it = pack[socket_fd];
    auto ret = realpath(it.path, it.real_path);
    it.real_path_len = strlen(it.real_path);
#ifdef DEBUG
    cout << "[debug] real path: " << it.real_path << endl;
#endif
    if (ret == nullptr) {
        if (errno == ENOENT || errno == ENOTDIR) {
            it.type = RESP_404;
        } else {
            it.type = RESP_500;
        }
        mod_send_socket_epoll(socket_fd);
        return true;
    }
    if (it.real_path_len < cwd_len) {
        it.type = RESP_500;
        mod_send_socket_epoll(socket_fd);
        return true;
    }
    for (size_t i = 0; i < cwd_len; i++) {
        if (it.real_path[i] != cwd[i]) {
            it.type = RESP_500;
            mod_send_socket_epoll(socket_fd);
            return true;
        }
    }
    return false;
}

void clean_pack(int socket_fd) {
    auto &it = pack[socket_fd];
    it.req_len = 0;
    it.recvd_len = 0;
    it.path = nullptr;
    it.path_len = 0;
    it.real_path_len = 0;
    it.file = nullptr;
    it.file_len = 0;
    bzero(&it.file_mtime, sizeof(it.file_mtime));
    it.file_fd = 0;
    it.read_len = 0;
    bzero(&it.file_cb, sizeof(it.file_cb));
    it.type = RESP_200;
    it.sent_len = 0;
    it.head_len = 0;
}


int close_socket(int socket_fd) {
#ifdef DEBUG
    cout << "[debug] close socket" << endl;
#endif
    del_socket_epoll(socket_fd);
    close(socket_fd);
    clean_pack(socket_fd);
    return 0;
}

struct cmp {
    bool operator()(const char *s1, const char *s2) const {
        return strcmp(s1, s2) == 0;
    }
};

struct hash_func {
    size_t operator()(const char *str) const {
        size_t len = strlen(str);
//        size_t _FNV_offset_basis = 14695981039346656037ULL;
//        size_t _FNV_prime = 1099511628211ULL;
//        size_t _Val = _FNV_offset_basis;
//        for (size_t _Next = 0; _Next < len; ++_Next) {
//            _Val ^= (size_t) str[_Next];
//            _Val *= _FNV_prime;
//        }
//        return _Val;
        size_t hash = 5381;
        while (len--){
            hash = ((hash << 5) + hash) + (*str++); /* hash * 33 + c */
        }
        return hash;
    }
};

#endif
