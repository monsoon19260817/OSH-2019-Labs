#ifndef BUILTIN_CPP
#define BUILTIN_CPP

#include "command.h"

set<string> builtin_cmd = {
        "cd",
        "alias",
        "export",
        "exit"
};

map<string, vector<string>> alias_map;

bool is_builtin(string &executable) {
    return builtin_cmd.find(executable) != builtin_cmd.end();
}

bool is_alias(string &str) {
    return alias_map.find(str) != alias_map.end();
}

int basic_command::exec_builtin() {
    if (executable == "cd") {
        return exec_cd();
    } else if (executable == "alias") {
        return exec_alias();
    } else if (executable == "export") {
        return exec_export();
    } else if (executable == "exit") {
        return exec_exit();
    } else {
        cerr << "wrong built-in command" << endl;
        return -1;
    }
}

int basic_command::exec_cd() {
    // todo cd 需要考虑 ~  ~username 这种写法
    if (!arg_list.empty()) {
//        char real_path[MAX_PATH_LEN];
//        if (realpath(arg_list[0].c_str(), real_path) == nullptr) {
//            // 未找到路径
//            cerr << "exec cd failed: can't find dir " << arg_list[0] << endl;
//            return -1;
//        }
        int ret = chdir(arg_list[0].c_str());
        if (ret == -1) {
            // 分析错误原因
//            cerr << "exec cd failed" << endl;
//            return -1;
            switch (errno) {
                case EACCES:
                    // 权限不足
                    cerr << "permission denied: " << arg_list[0] << endl;
                    break;
                case ENOTDIR:
                case ENOENT:
                    // 找不到文件
                    cerr << "no such file or directory: " << arg_list[0] << endl;
                    break;
                default:
                    cerr << "cd failed: " << arg_list[0] << endl;
                    break;
            }
        }
    } else {
        cerr << "exec cd failed: no args" << endl;
        return -1;
    }
    return 0;
}

int basic_command::exec_alias() {
    // 没有参数则直接显示所有
    if (arg_list.empty()) {
        for (auto &it:alias_map) {
            cout << it.first << '=';
            if (it.second.size() == 1) {
                cout << it.second[0];
            } else {
                cout << '\'';
                cout << it.second[0];
                for (int i = 1; i < it.second.size(); ++i) {
                    cout << ' ' << it.second[i];
                }
                cout << '\'';
            }
            cout << endl;
        }
        return 0;
    }
    int ret = 0;
    // 有等号的参数设置 alias
    // 无等号的参数显示 alias
    for (auto &it:arg_list) {
        int eq_index = find_eq(it);
        if (eq_index == -1) {
            // 无等号
            if (is_alias(it)) {
                cout << it << '=';
                auto cont = alias_map[it];
                if (cont.size() == 1) {
                    cout << cont[0];
                } else {
                    cout << '\'';
                    cout << cont[0];
                    for (int i = 1; i < cont.size(); ++i) {
                        cout << ' ' << cont[i];
                    }
                    cout << '\'';
                }
                cout << endl;
            } else {
                ret = -1;
            }
        } else if (eq_index == 0) {
            cerr << "syntax error near =: empty alias name" << endl;
            ret = -1;
        } else {
            auto tmp = it.substr(eq_index + 1);
            alias_map[it.substr(0, eq_index)] = lex(tmp);
        }
    }
    return ret;
}

int basic_command::exec_export() {
    // 无参数则显示所有 environ
    int ret = 0;
    if (arg_list.empty()) {
        for (int i = 0; environ[i] != nullptr; ++i) {
            cout << environ[i] << endl;
        }
    } else {
        // 有参数 参数必须为 ...=[...] 的形式
        // 不是这种形式则忽略
        for (auto &it:arg_list) {
            int eq_index = find_eq(it);
            if (eq_index == -1) {
                continue;
            } else if (eq_index == 0) {
                cerr << "syntax error near =: empty variable name" << endl;
                ret = -1;
            } else {
                int ret2 = setenv(
                        it.substr(0, eq_index).c_str(),
                        it.substr(eq_index + 1).c_str(), 1);
                if (ret2 == -1) {
                    ret = -1;
                }
            }
        }
    }
    return ret;
}

int basic_command::exec_exit() {
    if (!arg_list.empty() && is_signed_number(arg_list[0])) {
        exit(atoi(arg_list[0].c_str()));
    } else {
        exit(0);
    }
}

#endif