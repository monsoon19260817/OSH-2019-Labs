#ifndef LISTEN_CPP
#define LISTEN_CPP

#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>

#include "const.h"
#include "epoll.h"

using namespace std;

void start_listen() {

    // 创建套接字
    // AF_INET: 使用 IPv4
    // SOCK_STREAM: 面向连接的数据传输方式
    // IPPROTO_TCP: 使用 TCP 协议
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serv_sock == -1) {
        cerr << "[error] create socket failed" << endl;
        exit(-1);
    }

    // 将套接字和指定的 IP、端口绑定
    // 用 0 填充 serv_addr （它是一个 sockaddr_in 结构体）
    struct sockaddr_in serv_addr{};
    memset(&serv_addr, 0, sizeof(serv_addr));

    // 设置 IPv4
    // 设置 IP 地址
    // 设置端口
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(BIND_IP_ADDR);
    serv_addr.sin_port = htons(BIND_PORT);

    // 绑定
    int ret = bind(serv_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (ret == -1) {
        cerr << "[error] bind socket failed" << endl;
        exit(-1);
    }

    // 使得 serv_sock 套接字进入监听状态，开始等待客户端发起请求
    ret = listen(serv_sock, MAX_CONN);
    if (ret == -1) {
        cerr << "[error] start listening failed" << endl;
        exit(-1);
    }
    cout << "[info] start listening at " << BIND_IP_ADDR << ':' << BIND_PORT << endl;

    // 接收客户端请求，获得一个可以与客户端通信的新的生成的套接字 clnt_sock
    struct sockaddr_in clnt_addr{};
    socklen_t clnt_addr_size = sizeof(clnt_addr);

    for (;;) {
        // 当没有客户端连接时， accept() 会阻塞程序执行，直到有客户端连接进来
        int clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_addr, &clnt_addr_size);

#ifdef DEBUG
        cout << "[debug] accept success, socket=" << clnt_sock << endl;
#endif

        if (clnt_sock == -1) {
            cerr << "[error] accept connection failed" << endl;
            continue;
        }

        // 设置成非阻塞模式
        int flags = fcntl(clnt_sock, F_GETFL, 0);
        if (flags == -1) {
            cerr << "[error] get socket flags failed" << endl;
            close(clnt_sock);
            continue;
        }
        ret = fcntl(clnt_sock, F_SETFL, flags | O_NONBLOCK);
        if (ret == -1) {
            cerr << "[error] set socket as non-blocking failed" << endl;
            close(clnt_sock);
            continue;
        }
        // 放入 epoll 中
        pack[clnt_sock].recvd_len = 0;
        pack[clnt_sock].sent_len = 0;
        pack[clnt_sock].socket_fd = clnt_sock;
        add_recv_socket_epoll(clnt_sock);
    }
}

#endif

