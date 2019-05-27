#ifndef READ_FILE_CPP
#define READ_FILE_CPP

#include <aio.h>
#include <mutex>
//#include <unordered_map>
//#include <list>

using namespace std;

//mutex w_lock;
//unordered_map<char *, int, hash_func, cmp> reading_file;

void read_complete(sigval_t sigval) {
#ifdef DEBUG
    cout << "[debug] aio callback" << endl;
#endif
    int socket_fd = *((int *) sigval.sival_ptr);
    auto &it = pack[socket_fd];

#ifdef DEBUG
    cout << it.path << '\n' << it.file;
#endif

    if (aio_error(&it.file_cb) == 0) {
        // 更新 cache
        add_to_NORMAL(socket_fd);
        it.head_len = sprintf(it.head, HEAD_200, it.file_len);
#ifdef DEBUG
        cout << "[debug] head:\n" << it.head << endl;
        cout << "[debug] head len: " << it.head_len << endl;
#endif
        it.type = RESP_200;
        mod_send_socket_epoll(socket_fd);

//        w_lock.lock();
//        auto queue = reading_file.find(it.path);
//        if (queue == reading_file.end()) {
//            w_lock.unlock();
//            return;
//        }
//        reading_file.erase(queue);
//        w_lock.unlock();

        // 发送所有等待发送的 socket
//        auto &wait_list = pack[queue->second].wait_list;
//        for (auto &t:wait_list) {
//            pack[t].file = it.file;
//            pack[t].type = RESP_200;
//            mod_send_socket_epoll(t);
//        }
//        wait_list.clear();

    } else {
        // 读文件出错
        cerr << "[error] read file failed" << endl;
        delete[] it.file;
        it.type = RESP_500;
        mod_send_socket_epoll(socket_fd);
    }

}

int start_read_file(int socket_fd) {
    auto &it = pack[socket_fd];

//    w_lock.lock();
//    auto queue = reading_file.find(it.path);
//    if (queue == reading_file.end()) {
//        reading_file[it.path] = socket_fd;
//    } else {
//        w_lock.unlock();
//        pack[queue->second].wait_list.push_back(socket_fd);
//        return 0;
//    }
//    w_lock.unlock();

    // 非阻塞模式
    it.file_fd = open(it.path, O_RDONLY | O_NONBLOCK);
    if (it.file_fd < 0) {
        // 打开文件失败
        if (errno == ENOENT) {
            it.type = RESP_404;
            mod_send_socket_epoll(socket_fd);
        } else {
            it.type = RESP_500;
            mod_send_socket_epoll(socket_fd);
        }
        return -1;
    }
    it.read_len = 0;
    it.file = (char *) malloc(it.file_len);
    if (it.file == nullptr) {
        // 内存不足
        cerr << "[error] malloc failed" << endl;
        it.type = RESP_500;
        mod_send_socket_epoll(socket_fd);
        return -1;
    }

    bzero(&it.file_cb, sizeof(it.file_cb));
    it.file_cb.aio_fildes = it.file_fd;
    it.file_cb.aio_buf = it.file;
    it.file_cb.aio_nbytes = it.file_len;
    // callback
    it.file_cb.aio_sigevent.sigev_notify = SIGEV_THREAD;
    it.file_cb.aio_sigevent.sigev_notify_function = read_complete;
    it.file_cb.aio_sigevent.sigev_notify_attributes = nullptr;
    it.file_cb.aio_sigevent.sigev_value.sival_ptr = &it.socket_fd;

    int ret = aio_read(&it.file_cb);
    if (ret != 0) {
        cerr << "start read file failed" << endl;
        it.type = RESP_500;
        mod_send_socket_epoll(socket_fd);
        return -1;
    }
#ifdef  DEBUG
    cout << "[debug] start file aio read" << endl;
#endif
    return 0;
}

#endif