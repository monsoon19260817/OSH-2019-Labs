#ifndef RECEIVE_CPP
#define RECEIVE_CPP

#include <errno.h>

#include "epoll.h"
#include "parse.cpp"
#include "const.h"

using namespace std;

/**
 * 如果出现 \0 则返回 -2
 * 未出现 \r 则返回 -1
 * 出现 \r 则返回 index
 */
ssize_t find_new_line(const char *str, size_t start, size_t len) {
    for (size_t i = start; i < len; i++) {
        if (str[i] == '\r') {
            return i;
        } else if (str[i] == '\0') {
            return -2;
        }
    }
    return -1;
}

bool is_end(const char *req, size_t start, size_t len) {
    return len > start + 3 &&
           req[start] == '\r' &&
           req[start + 1] == '\n' &&
           req[start + 2] == '\r' &&
           req[start + 3] == '\n';
}

int receive(int socket_fd) {

    auto &it = pack[socket_fd];

#ifdef DEBUG
    cout << "[debug] start receive" << endl;
#endif

    for (;;) {
        ssize_t n = read(socket_fd, it.req + it.recvd_len, it.max_req_len - it.recvd_len);

        if (n < 0) {
#ifdef DEBUG
            cout << "[debug] receive from socket failed" << endl;
#endif
            if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) {
#ifdef DEBUG
                cout << "[debug] again" << endl;
#endif
                return 0;
            } else {
                // 接收失败
                cerr << "[error] receive failed" << endl;
                close_socket(socket_fd);
                return -1;
            }
        } else {
#ifdef DEBUG
            cout << "[debug] received from socket" << endl;
#endif
            it.req_len += n;

            // 判断是否已经读完
            ssize_t index = it.req_len - n - 3;    // 寻找 \r
            if (index < 0) {
                index = 0;
            }
            while (index < it.req_len) {
                index = find_new_line(it.req, index, it.req_len);
                if (index == -2) {
                    // 出现 \0
                    close_socket(socket_fd);
                    return -1;
                } else if (index == -1) {
                    // 找不到 \r
                    break;
                }
                if (is_end(it.req, index, it.req_len)) {
                    // 找到了 \r\n\r\n 接收结束
#ifdef DEBUG
                    cout << "[debug] request:\n" << it.req << endl;
#endif
                    return parse(socket_fd);
                } else {
                    index++;
                }
            }

            if (it.req_len == it.max_req_len) {
                // 空间不足
                it.max_req_len += MAX_RECV_LEN;
                auto ptr = realloc(it.req, it.max_req_len);
                if (ptr == nullptr) {
                    cerr << "[error] realloc failed" << endl;
                    close_socket(socket_fd);
                    return -1;
                }
                it.req = (char *) ptr;
            }
            return 0;
        }
    }
}

#endif

