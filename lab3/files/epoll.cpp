#ifndef EPOLL_CPP
#define EPOLL_CPP

#include <sys/epoll.h>

#include "epoll.h"
#include "receive.cpp"
#include "send.cpp"

using namespace std;

int epfd;

void init_epoll() {
    epfd = epoll_create(EPOLL_SIZE);
    if (epfd == -1) {
        cerr << "[error] create epoll failed" << endl;
        exit(-1);
    }
    cout << "[info] create epoll success" << endl;
}

void run_epoll() {

    struct epoll_event events[EPOLL_SIZE];
    for (;;) {
#ifdef DEBUG
        cout << "[debug] waiting epoll" << endl;
#endif
        // timeout 10 s
        int n = epoll_wait(epfd, events, EPOLL_SIZE, -1);
#ifdef DEBUG
        cout << "[debug] get epoll event" << endl;
#endif
        if (n == -1) {
            cerr << "[error] wait epoll failed" << endl;
            exit(-1);
        }
        for (int i = 0; i < n; i++) {
            // socket_fd 作为 key 获得数据
            int socket_fd = events[i].data.u32;
            auto &it = pack[socket_fd];
            if (events[i].events & EPOLLIN) {
#ifdef DEBUG
                cout << "[debug] get socket receive" << endl;
#endif
                // 接收
                receive(socket_fd);
            } else if (events[i].events & EPOLLOUT) {
                // 发送
#ifdef DEBUG
                cout << "[debug] get socket send" << endl;
#endif
                switch (it.type) {
                    case RESP_200:
                        send_200(socket_fd);
                        break;
                    case RESP_404:
                        send_404(socket_fd);
                        break;
                    case RESP_500:
                        send_500(socket_fd);
                        break;
                }
            } else {
                cerr << "[error] undefined socket" << endl;
                close(it.socket_fd);
            }
        }
    }
}

int add_recv_socket_epoll(int socket_fd) {
    struct epoll_event ev{};
    ev.events = EPOLLIN | EPOLLET;
    ev.data.u32 = socket_fd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, socket_fd, &ev);
    if (ret == -1) {
        cerr << "[error] add receive socket to epoll failed" << endl;
        close_socket(socket_fd);
        return -1;
    }
#ifdef DEBUG
    cout << "[debug] add receive socket to epoll" << endl;
#endif
    return 0;
}

int mod_send_socket_epoll(int socket_fd) {
    struct epoll_event ev{};
    ev.events = EPOLLOUT | EPOLLET;
    ev.data.u32 = socket_fd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_MOD, socket_fd, &ev);
    if (ret == -1) {
        cerr << "[error] change socket to send failed" << endl;
        close_socket(socket_fd);
        return -1;
    }
#ifdef DEBUG
    cout << "[debug] mod send socket in epoll" << endl;
#endif
    return 0;
}

int del_socket_epoll(int socket_fd) {
    int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, socket_fd, nullptr);
    if (ret == -1) {
        cerr << "[error] delete socket from epoll failed" << endl;
        return -1;
    }
#ifdef DEBUG
    cout << "[debug] delete socket from epoll" << endl;
#endif
    return 0;
}

#endif