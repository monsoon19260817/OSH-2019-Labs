#ifndef IO_MOD_CPP
#define IO_MOD_CPP

int io_mod::apply() {
    if (type == ">") {
        int fd = open(str.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0666);
        if (fd == -1) {
            // todo 分析错误原因
            cerr << "[error] open file failed: " << str << endl;
            return -1;
        }
        int ret = dup2(fd, num1);
        if (ret == -1) {
            cerr << "[error] dup failed" << endl;
            return -1;
        }
        ret = close(fd);
        if (ret == -1) {
            // todo 分析错误原因
            cerr << "[error] close file failed: " << str << endl;
            return -1;
        }
    } else if (type == ">>") {
        int fd = open(str.c_str(), O_WRONLY | O_APPEND | O_CREAT, 0666);
        if (fd == -1) {
            // todo 分析错误原因
            cerr << "[error] open file failed: " << str << endl;
            return -1;
        }
        int ret = dup2(fd, num1);
        if (ret == -1) {
            cerr << "[error] dup failed" << endl;
            return -1;
        }
        ret = close(fd);
        if (ret == -1) {
            // todo 分析错误原因
            cerr << "[error] close file failed: " << str << endl;
            return -1;
        }
    } else if (type == "<") {
        int fd = open(str.c_str(), O_RDONLY);
        if (fd == -1) {
            // todo 分析错误原因
            cerr << "[error] open file failed: " << str << endl;
            return -1;
        }
        int ret = dup2(fd, num1);
        if (ret == -1) {
            cerr << "[error] dup failed" << endl;
            return -1;
        }
        ret = close(fd);
        if (ret == -1) {
            // todo 分析错误原因
            cerr << "[error] close file failed: " << str << endl;
            return -1;
        }
    } else if (type == ">&" || type == "<&") {
        int ret = dup2(num2, num1);
        if (ret == -1) {
            cerr << "[error] dup failed" << endl;
            return -1;
        }
    }
//    else if (type == "<<") {
//        string in;
//        for (;;) {
//            string s = readline("heredoc> ");
//            if (s == str + '\n') {
//                break;
//            } else {
//                in += s;
//            }
//        }
//        int ret = write(STDIN_FILENO, in.c_str(), in.size());
//        if (ret == -1) {
//            cerr << "[error] exec << failed" << endl;
//            return -1;
//        }
//    } else if (type == "<<<") {
//        str += '\n';
//        int ret = write(STDIN_FILENO, str.c_str(), str.size());
//        if (ret == -1) {
//            cerr << "[error] exec <<< failed" << endl;
//            return -1;
//        }
//    } else {
//        cerr << "apply io modify failed" << endl;
//        return -1;
//    }
    return 0;
}

#endif

