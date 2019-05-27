#ifndef SEND_CPP
#define SEND_CPP

#include "epoll.h"

int send_200(int socket_fd) {

    auto &it = pack[socket_fd];

    for (;;) {
#ifdef DEBUG
        cout << "[debug] sent len: " << it.sent_len << endl;
        cout << "[debug] head len: " << it.head_len << endl;
#endif
        ssize_t n;
        if (it.sent_len < it.head_len) {
            // 发送头部
            n = write(socket_fd, it.head + it.sent_len, it.head_len - it.sent_len);
#ifdef DEBUG
            cout << "[debug] send head" << endl;
#endif
        } else {
            // 发送文件内容
            n = write(socket_fd,
                      it.file + (it.sent_len - it.head_len),
                      it.file_len - (it.sent_len - it.head_len));
#ifdef DEBUG
            cout << "[debug] send file" << endl;
#endif
        }

#ifdef DEBUG
        cout << "n = " << n << endl;
#endif
        if (n < 0) {
            if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) {
                return 0;
            } else {
                cerr << "[error] send failed" << endl;
                close_socket(socket_fd);
                return -1;
            }
        } else {
            it.sent_len += n;
            if (it.sent_len == it.head_len + it.file_len) {
                // 全部发送完毕
                close_socket(socket_fd);
                return 0;
            }
        }
    }
    return 0;
}


int send_404(int socket_fd) {
    auto &it = pack[socket_fd];

    for (;;) {
        ssize_t n = write(socket_fd, HEAD_404 + it.sent_len, HEAD_404_LEN - it.sent_len);
#ifdef DEBUG
        cout << "n = " << n << endl;
#endif
        if (n < 0) {
            if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) {
                return 0;
            } else {
                cerr << "[error] send failed" << endl;
                close_socket(socket_fd);
                return -1;
            }
        } else {
            it.sent_len += n;
            if (it.sent_len == HEAD_404_LEN) {
                // 全部发送完毕
                close_socket(socket_fd);
                return 0;
            }
        }
    }
    return 0;
}

int send_500(int socket_fd) {
    auto &it = pack[socket_fd];

    for (;;) {
        ssize_t n = write(socket_fd, HEAD_500 + it.sent_len, HEAD_500_LEN - it.sent_len);
#ifdef DEBUG
        cout << "n = " << n << endl;
#endif
        if (n < 0) {
            if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) {
                return 0;
            } else {
                cerr << "[error] send failed" << endl;
                close_socket(socket_fd);
                return -1;
            }
        } else {
            it.sent_len += n;
            if (it.sent_len == HEAD_500_LEN) {
                // 全部发送完毕
                close_socket(socket_fd);
                return 0;
            }
        }
    }
    return 0;
}

#endif
