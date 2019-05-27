#include <csignal>
#include <thread>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

//#define DEBUG

using namespace std;

#include "utils.cpp"
#include "const.h"
#include "listen.cpp"
#include "epoll.cpp"

void SIGINT_handler(int s) {
    cerr << "\n[error] SIGINT received" << endl;
    exit(s);
}

int main() {
    signal(SIGINT, SIGINT_handler);
    signal(SIGPIPE, SIG_IGN);

    for (int i = 0; i < PACK_NUM; i++) {
        clean_pack(i);
        pack[i].req = (char *) malloc(MAX_RECV_LEN);
        if (pack[i].req == nullptr) {
            cerr << "[error] malloc failed" << endl;
            exit(-1);
        }
        pack[i].max_req_len = MAX_RECV_LEN;
        pack[i].head = (char *) malloc(HEAD_200_LEN + 10);
        if (pack[i].head == nullptr) {
            cerr << "[error] malloc failed" << endl;
            exit(-1);
        }
        pack[i].real_path = (char *) malloc(MAX_PATH_LEN);
        if (pack[i].real_path == nullptr) {
            cerr << "[error] malloc failed" << endl;
            exit(-1);
        }
    }

    init_utils();
    init_epoll();
    thread t1(run_epoll);
    start_listen();
    return 0;
}