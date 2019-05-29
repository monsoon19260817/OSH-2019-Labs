#ifndef SHELL_PIPE_COMMAND_CPP
#define SHELL_PIPE_COMMAND_CPP

#include "command.h"

pipe_command::pipe_command(vector<vector<string>> &str) {
    type = PIPE_CMD;
    for (auto &it:str) {
        if (is_basic_cmd(it)) {
            auto cmd = new basic_command(it);
            if (cmd->is_bg) {
                throw "syntax error: sub cmd of pipe cmd can't be background";
            }
            subcmd_list.push_back(cmd);
        } else {
            throw "syntax error: sub cmd of pipe cmd isn't basic cmd";
        }
    }
}

int pipe_command::exec() {
#ifdef DEBUG
    cout << "[debug] exec pipe cmd" << endl;
#endif
    // 创建 pipe
    int pipefds[subcmd_list.size() - 1][2];
    for (int i = 0; i < subcmd_list.size() - 1; ++i) {
        int ret = pipe(pipefds[i]);
        if (ret == -1) {
            cerr << "[error] create pipe failed" << endl;
            return -1;
        }
    }
    for (int i = 0; i < subcmd_list.size(); ++i) {
        auto t = fork();
        if (t == -1) {
            cerr << "[error] fork failed" << endl;
            return -1;
        } else if (t == 0) {
            int ret2 = 0;

            // 关闭子进程中不需要的 pipe
            for (int j = 0; j < subcmd_list.size() - 1; ++j) {
                if (j != i - 1) {
                    int ret = close(pipefds[j][0]);
                    if (ret == -1) {
                        cerr << "close pipe fd failed" << endl;
                        exit(-1);
                    }
                }
                if (j != i) {
                    int ret = close(pipefds[j][1]);
                    if (ret == -1) {
                        cerr << "close pipe fd failed" << endl;
                        exit(-1);
                    }
                }
            }

            // 重定向
            if (i > 0) {
                int ret = dup2(pipefds[i - 1][0], STDIN_FILENO);
                if (ret == -1) {
                    cerr << "dup failed" << endl;
                    exit(-1);
                }
            }
            if (i < subcmd_list.size() - 1) {
                int ret = dup2(pipefds[i][1], STDOUT_FILENO);
                if (ret == -1) {
                    cerr << "dup failed" << endl;
                    exit(-1);
                }
            }

            if (i == 0) {
                ret2 = subcmd_list[i]->exec();
            } else if (i == subcmd_list.size() - 1) {
                ret2 = subcmd_list[i]->exec();
            } else {
                ret2 = subcmd_list[i]->exec();
            }

            if (i > 0) {
                int ret = close(pipefds[i - 1][0]);
                if (ret == -1) {
                    cerr << "close pipe fd failed" << endl;
                    exit(-1);
                }
            }
            if (i < subcmd_list.size() - 1) {
                int ret = close(pipefds[i][1]);
                if (ret == -1) {
                    cerr << "close pipe fd failed" << endl;
                    exit(-1);
                }
            }

            exit(ret2);
        }
    }
    // 父进程中无需使用管道
    for (int i = 0; i < subcmd_list.size() - 1; ++i) {
        int ret = close(pipefds[i][0]);
        if (ret == -1) {
            cerr << "close pipe fd failed" << endl;
            return -1;
        }
        ret = close(pipefds[i][1]);
        if (ret == -1) {
            cerr << "close pipe fd failed" << endl;
            return -1;
        }
    }
    int ret = 0;
    if (!is_bg) {
        // todo 获得子进程返回值
        // 有多个子进程
//        for (int i = 0; i < subcmd_list.size(); ++i) {
//            int ret2 = 0;
//            wait(&ret2);
//            if (ret2 != 0) {
//                ret = ret2;
//            }
//        }
//        wait(NULL);

        while (auto pid = waitpid(-1, NULL, 0)) {
            if (errno == ECHILD) {
                break;
            }
        }
    }
    return ret;
}

pipe_command::~pipe_command() {
    for (auto &it:subcmd_list) {
        delete it;
    }
}

#endif
