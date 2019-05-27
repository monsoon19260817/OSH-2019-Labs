#ifndef EPOLL_H
#define EPOLL_H

void init_epoll();

void run_epoll();

int add_recv_socket_epoll(int socket_fd);

int mod_send_socket_epoll(int socket_fd);

int del_socket_epoll(int socket_fd);

#endif
