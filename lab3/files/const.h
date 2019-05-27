#ifndef CONST_H
#define CONST_H

#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <aio.h>

const char *BIND_IP_ADDR = "0.0.0.0"; // 监听地址
const int BIND_PORT = 8000;             // 监听端口
const int MAX_RECV_LEN = 1024;       // 最大接收长度
const int MAX_PATH_LEN = 1024;          // 最大路径长度
const int MAX_CONN = 10000;              // 最大连接数
const int EPOLL_SIZE = 100;
const int CPU_NUM = 8;
const int PACK_NUM = 20000;
const int TIMEOUT = 10000;  // 10s

const char *HEAD_200 = "HTTP/1.0 200 OK\r\nContent-Length: %zu\r\n\r\n";
const char *HEAD_404 = "HTTP/1.0 404 Not Found\r\nContent-Length: 0\r\n\r\n";
const char *HEAD_500 = "HTTP/1.0 500 Internal Server Error\r\nContent-Length: 0\r\n\r\n";

const int HEAD_200_LEN = strlen(HEAD_200);
const int HEAD_404_LEN = strlen(HEAD_404);
const int HEAD_500_LEN = strlen(HEAD_500);

#endif
