#ifndef SHELL_GROUP_COMMAND_CPP
#define SHELL_GROUP_COMMAND_CPP

#include "command.h"

group_command::group_command(vector<vector<string>> &str) {
    type = GROUP_CMD;
    for (auto &it:str) {
        subcmd_list.push_back(parse(it));
    }
}

int group_command::exec() {
#ifdef DEBUG
    cout << "[debug] exec group cmd" << endl;
#endif
    int ret = 0;
    if (is_bg) {
        auto t = fork();
        if (t == -1) {
            cerr << "[error] fork failed" << endl;
            return -1;
        } else if (t == 0) {
            for (auto &it:subcmd_list) {
                int ret2 = it->exec();
                if (ret == 0) {
                    ret = ret2;
                }
            }
            exit(ret);
        }
        // 获得子进程返回值
        ret = 0;
//        wait(&ret);
//        return ret;
//        wait(nullptr);
    } else {
        for (auto &it:subcmd_list) {
            int ret2 = it->exec();
            if (ret == 0) {
                ret = ret2;
            }
        }
    }
    return ret;
}

group_command::~group_command() {
    for (auto &it:subcmd_list) {
        delete it;
    }
}

#endif