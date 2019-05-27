#ifndef PARSE_CPP
#define PARSE_CPP

#include "cache.cpp"
#include "const.h"
#include "read_file.cpp"

int parse(int socket_fd) {
    auto &it = pack[socket_fd];

    if (it.req == nullptr || it.req_len <= 0) {
        // 未收到请求
        cerr << "[error] wrong request" << endl;
        return -1;
    }

    // 只取第一行分析
    for (size_t i = 0; i < it.req_len; ++i) {
        if (it.req[i] == '\r') {
            it.req[i] = '\0';
            it.req_len = i;
            break;
        }
    }

    size_t s1 = 0;
    while (s1 < it.req_len && it.req[s1] != ' ') {
        s1++;
    }
    s1++; // s1 定于开始
    if (s1 >= it.req_len || it.req[s1] != '/') {
        // 找不到路径
        it.type = RESP_500;
        mod_send_socket_epoll(socket_fd);
        return -1;
    }
    size_t s2 = s1;
    while (s2 < it.req_len && it.req[s2] != ' ') {
        s2++;
    }
    // s2 定于结尾空格

    if (s2 == s1) {
        // 找不到路径
        it.type = RESP_500;
        mod_send_socket_epoll(socket_fd);
        return -1;
    }
#ifdef DEBUG
    cout << "[debug] request:\n" << it.req << endl;
#endif

    // 检查是不是符合 GET path HTTP/x.x
    it.req[s1 - 1] = '\0';
    it.req[s2] = '\0';
    it.req[s2 + 6] = '\0';
    if (strcmp("GET", it.req) != 0 || strcmp("HTTP/", it.req + s2 + 1) != 0) {
        // 错误请求格式
#ifdef DEBUG
        cout << "[debug] wrong request" << endl;
#endif
        it.type = RESP_500;
        mod_send_socket_epoll(socket_fd);
        return -1;
    }
#ifdef DEBUG
    cout << "[debug] right request" << endl;
#endif
    s1--;
    it.req[s1] = '.';
    it.path_len = s2 - s1;
    it.path = it.req + s1;
    it.path[it.path_len] = '\0';

    // 去除结尾的 /
    if (it.path_len > 0 && it.path[it.path_len - 1] == '/') {
        it.path_len--;
        it.path[it.path_len] = '\0';
    }

#ifdef DEBUG
    cout << "[debug] path: " << it.path << endl;
#endif

    if (is_escape(socket_fd)) {
#ifdef DEBUG
        cout << "[debug] file escape" << endl;
#endif
        return -1;
    }

    auto state = update_cache(socket_fd);
    switch (state) {
        case HIT:
            if (it.file == nullptr) {
                it.type = RESP_404;
                mod_send_socket_epoll(socket_fd);
            } else {
                it.head_len = sprintf(it.head, HEAD_200, it.file_len);
                it.type = RESP_200;
                mod_send_socket_epoll(socket_fd);
            }
            return 0;
        case MISS:
            // 加入 aio
            return start_read_file(socket_fd);
        case EXCEPTION:
            it.type = RESP_500;
            mod_send_socket_epoll(socket_fd);
            return 0;
    }

    return -1;
}

#endif