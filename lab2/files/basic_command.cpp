#ifndef SHELL_BASIC_COMMAND_CPP
#define SHELL_BASIC_COMMAND_CPP

#include "command.h"
#include "builtin.cpp"
#include "io_mod.cpp"

/**
 * 基本命令格式
 * [envset ... ] cmd [arg ... ] [io mods] [&]
 */
basic_command::basic_command(vector<string> &str) {
#ifdef DEBUG
    cout << "[debug] === start create basic cmd ===" << endl;
    cout << '\t';
    for (auto &it:str) {
        cout << it << '\t';
    }
    cout << "\n[debug] ==============================\n" << endl;
#endif
    type = BASIC_CMD;

    int index = 0;   // 当前还未解析的地方

    // 找到所有 envset
    for (auto &it:str) {
        int eq_index = find_eq(it);
        if (eq_index == -1) {
            break;
        } else if (eq_index == 0) {
            throw "syntax error near =: empty var name";
        } else {
            env_list[it.substr(0, eq_index)] = it.substr(eq_index + 1);
            index++;
        }
    }

    if (index >= str.size()) {
        // 未找到命令
        throw "syntax error: can't find executable";
    }

    // 替换 alias
    if (is_alias(str[index])) {
#ifdef DEBUG
        cout << "[debug] replace alias " << str[index] << '\n' << endl;
#endif
        auto cont = alias_map[str[index]];
        str.erase(str.begin() + index);
        str.insert(str.begin() + index, cont.begin(), cont.end());
#ifdef DEBUG
        cout << "[debug] == replace alias finished ==" << endl;
        cout << '\t';
        for (auto &it:str) {
            cout << it << '\t';
        }
        cout << "\n[debug] ============================\n" << endl;
#endif
    }

    // 找到命令
    if (!is_key_word(str[index])) {
        executable = str[index];
        index++;
    } else {
        throw ("syntax error near " + str[index] + ": can't find executable").c_str();
    }

    // 找到所有 io key word
    auto io_indexs = find_all_io_key_word(str, index);

    int arg_start = index;
    if (io_indexs.empty()) {
        // 没有 io 修饰
        // 找到 args
        for (int i = arg_start; i < str.size() - 1; ++i) {
            arg_list.push_back(str[i]);
            index++;
        }
        // 判断是否是后台命令
        if (index == str.size() - 1) {
            if (str[index] == "&") {
                is_bg = true;
            } else {
                arg_list.push_back(str[index]);
            }
        }

#ifdef DEBUG
        cout << "[debug] === basic cmd created ===" << endl;
        cout << "\texecutable: " << executable << endl;
        if (!env_list.empty()) {
            cout << "\tenv set list:" << endl;
            for (auto &it:env_list) {
                cout << "\t\t" << it.first << '=' << it.second << endl;
            }
        }
        if (!arg_list.empty()) {
            cout << "\targ list:" << endl;
            for (auto &it:arg_list) {
                cout << "\t\t" << it << endl;
            }
        }
        if (!io_list.empty()) {
            cout << "\tio mod list:" << endl;
            for (auto &it:io_list) {
                cout << "\t\t" << it.num1 << ' ' << it.type << ' ' << it.num2 << ' ' << it.str << endl;
            }
        }
        if (is_bg) {
            cout << "\tbackground\n";
        }
        cout << "[debug] =========================\n" << endl;
#endif

        return;
    }

    // 有 io 修饰
    int arg_end = io_indexs[0];
    if (str[arg_end] == ">" || str[arg_end] == ">>" || str[arg_end] == "<" ||
        str[arg_end] == ">&" || str[arg_end] == "<&") {
        if (index <= arg_end - 1 && is_number(str[arg_end - 1])) {
            arg_end--;
        }
    }
    for (int i = arg_start; i < arg_end; ++i) {
        arg_list.push_back(str[i]);
    }
    index = arg_end;

    // 解析 io 修饰
    for (auto &it:io_indexs) {
        if (str[it] == ">") {
            if (it + 1 < str.size() && !is_key_word(str[it + 1])) {
                auto tmp = io_mod();
                tmp.type = ">";
                tmp.str = str[it + 1];
                if (index == it) {
                    tmp.num1 = STDOUT_FILENO;
                    index += 2;
                } else if (index == it - 1) {
                    if (is_number(str[it - 1])) {
                        tmp.num1 = atoi(str[it - 1].c_str());
                    } else {
                        throw "syntax error near >";
                    }
                    index += 3;
                } else {
                    throw "syntax error near >";
                }
                io_list.push_back(tmp);
            } else {
                throw "syntax error near >";
            }
        } else if (str[it] == ">>") {
            if (it + 1 < str.size() && !is_key_word(str[it + 1])) {
                auto tmp = io_mod();
                tmp.type = ">>";
                tmp.str = str[it + 1];
                if (index == it) {
                    tmp.num1 = STDOUT_FILENO;
                    index += 2;
                } else if (index == it - 1) {
                    if (is_number(str[it - 1])) {
                        tmp.num1 = atoi(str[it - 1].c_str());
                    } else {
                        throw "syntax error near >>";
                    }
                    index += 3;
                } else {
                    throw "syntax error near >>";
                }
                io_list.push_back(tmp);
            } else {
                throw "syntax error near >>";
            }
        } else if (str[it] == "<") {
            if (it + 1 < str.size() && !is_key_word(str[it + 1])) {
                auto tmp = io_mod();
                tmp.type = "<";
                tmp.str = str[it + 1];
                if (index == it) {
                    tmp.num1 = STDIN_FILENO;
                    index += 2;
                } else if (index == it - 1) {
                    if (is_number(str[it - 1])) {
                        tmp.num1 = atoi(str[it - 1].c_str());
                    } else {
                        throw "syntax error near <";
                    }
                    index += 3;
                } else {
                    throw "syntax error near <";
                }
                io_list.push_back(tmp);
            } else {
                throw "syntax error near <";
            }
        } else if (str[it] == ">&") {
            if (it + 1 < str.size() && is_number(str[it + 1])) {
                auto tmp = io_mod();
                tmp.type = ">&";
                tmp.num2 = atoi(str[it + 1].c_str());
                if (index == it) {
                    tmp.num1 = STDOUT_FILENO;
                    index += 2;
                } else if (index == it - 1) {
                    if (is_number(str[it - 1])) {
                        tmp.num1 = atoi(str[it - 1].c_str());
                    } else {
                        throw "syntax error near >&";
                    }
                    index += 3;
                } else {
                    throw "syntax error near >&";
                }
                io_list.push_back(tmp);
            } else {
                throw "syntax error near >&";
            }
        } else if (str[it] == "<&") {
            if (it + 1 < str.size() && is_number(str[it + 1])) {
                auto tmp = io_mod();
                tmp.type = "<&";
                tmp.num2 = atoi(str[it + 1].c_str());
                if (index == it) {
                    tmp.num1 = STDIN_FILENO;
                    index += 2;
                } else if (index == it - 1) {
                    if (is_number(str[it - 1])) {
                        tmp.num1 = atoi(str[it - 1].c_str());
                    } else {
                        throw "syntax error near <&";
                    }
                    index += 3;
                } else {
                    throw "syntax error near <&";
                }
                io_list.push_back(tmp);
            } else {
                throw "syntax error near <&";
            }
        }
//        else if (str[it] == "<<") {
//            if (it == index && it + 1 < str.size() && !is_key_word(str[it + 1])) {
//                auto tmp = io_mod();
//                tmp.type = "<<";
//                tmp.str = str[it + 1];
//                index += 2;
//            } else {
//                throw "syntax error near <<";
//            }
//        } else if (str[it] == "<<<") {
//            if (it == index && it + 1 < str.size() && !is_key_word(str[it + 1])) {
//                auto tmp = io_mod();
//                tmp.type = "<<<";
//                tmp.str = str[it + 1];
//                index += 2;
//            } else {
//                throw "syntax error near <<<";
//            }
//        } else {
//            throw ("syntax error near " + str[it] + ": wrong io key word").c_str();
//        }

    }

    // 结束
    if (index == str.size() - 1 && str[index] == "&") {
        is_bg = true;
    } else if (index < str.size() - 1) {
        throw ("syntax error near " + str[index]).c_str();
    }
#ifdef DEBUG
    cout << "[debug] === basic cmd created ===" << endl;
    cout << "\texecutable: " << executable << endl;
    if (!env_list.empty()) {
        cout << "\tenv set list:" << endl;
        for (auto &it:env_list) {
            cout << "\t\t" << it.first << '=' << it.second << endl;
        }
    }
    if (!arg_list.empty()) {
        cout << "\targ list:" << endl;
        for (auto &it:arg_list) {
            cout << "\t\t" << it << endl;
        }
    }
    if (!io_list.empty()) {
        cout << "\tio mod list:" << endl;
        for (auto &it:io_list) {
            cout << "\t\t" << it.num1 << ' ' << it.type << ' ' << it.num2 << ' ' << it.str << endl;
        }
    }
    if (is_bg) {
        cout << "\tbackground\n";
    }
    cout << "=================================" << endl;
#endif

}

int basic_command::exec() {
#ifdef DEBUG
    cout << "[debug] exec basic cmd" << endl;
#endif
    // 只有 基本 且 非内置 的命令才需要 fork
    // fork 是为了防止连带 shell 一起崩溃
    parse_all();        // 解析所有引号内容和 $ 变量

    // 替换 alias
    // 在 parse 第一个非 envset 时替换
//    if (is_alias(executable)) {
//        executable = alias_map[executable];
//    }
    // 内置命令直接运行
    if (is_builtin(executable)) {
        return exec_builtin();
    }
    // 外部命令 fork 运行
    auto t = fork();
    if (t == -1) {
        cerr << "[error] fork failed" << endl;
        return -1;
    } else if (t == 0) {
//        int now_bg_num = bg_num+1;
//        if (is_bg) {
//            bg_num++;
//            cout << "\n[" << now_bg_num << "] " << getpid() << endl;
//        }
        // 应用 io 重定向
        apply_io_mod();
        char *argv[arg_list.size() + 2];
        argv[0] = executable.data();
        for (int i = 0; i < arg_list.size(); ++i) {
            argv[i + 1] = (char *) arg_list[i].c_str();
        }
        argv[arg_list.size() + 1] = nullptr;
        int ret;
        if (env_list.empty()) {
            ret = execvp(executable.c_str(), argv);
        } else {
            // 传递变量
            int environ_len = 0;
            while (environ[environ_len] != nullptr) {
                environ_len++;
            }
            char *envp[environ_len + env_list.size() + 1];
            int i;
            for (i = 0; i < environ_len; ++i) {
                envp[i] = environ[i];
            }
            for (auto &it:env_list) {
                envp[i + environ_len] = (it.first + '=' + it.second).data();
                i++;
            }
            envp[environ_len + env_list.size()] = nullptr;
            ret = execvpe(executable.c_str(), argv, envp);
        }
        if (ret == -1) {
            // 根据 errno 检查错误
            switch (errno) {
                case EACCES:
                    // 权限不足
                    cerr << "permission denied: " << executable << endl;
                    break;
                case ENOTDIR:
                case ENOENT:
                    // 找不到文件
                    cerr << "command not found: " << executable << endl;
                    break;
                case ENOEXEC:
                    // 非法文件格式
                    cerr << "exec format error: " << executable << endl;
                    break;
                case ETXTBSY:
                    // 文件占用
                    cerr << "file in occupation: " << executable << endl;
                    break;
                case ENOMEM:
                    // 内存不足
                    cerr << "no enough memory: " << executable << endl;
                    break;
                default:
                    cerr << "exec failed: " << executable << endl;
            }
        }
//        if (is_bg) {
//            bg_num--;
//            cout << '[' << now_bg_num << "]  +" << getpid() <<" done"<< endl;
//        }
        exit(ret);
    }
    if (!is_bg) {
        // todo 获得子进程返回值
//        int ret = 0;
//        wait(&ret);
//        return ret;
//        wait(NULL);
        while (auto pid = waitpid(-1, NULL, 0)) {
            if (errno == ECHILD) {
                break;
            }
        }
    }
    return 0;
}


//string get_env_val(string &name) {
//    for (int i = 0; environ[i] != nullptr; ++i) {
//        if (strstr(environ[i], name.c_str()) == environ[i] &&
//            environ[i][name.size()] == '=') {
//            return string(environ[i] + name.size() + 1);
//        }
//    }

//    char *var = getenv(name.c_str());
//    if (var == nullptr) {
//        return string("");
//    } else {
//        return string(var);
//    }
//
//}

int find_quota_interval(
        const string &str, vector<int> &start, vector<int> &end) {
    bool in_quota = false;
    char quota_char;
    for (int i = 0; i < str.size(); ++i) {
        if (in_quota) {
            if (str[i] == quota_char) {
                in_quota = false;
                end.push_back(i + 1);
            }
        } else {
            if (str[i] == '\'' || str[i] == '\"') {
                in_quota = true;
                quota_char = str[i];
                start.push_back(i);
            }
        }
    }
    if (in_quota == true) {
        cerr << "syntax error: " << quota_char << " isn't closed" << endl;
        return -1;
    }
    return 0;
}

const regex reg("\\$\\w+");

string parse_var(const string &str) {
    // todo

    vector<int> indexs, lens;  // 每个匹配结果的 开始位置和长度
    smatch mat;

    auto start = str.begin();
    auto end = str.end();
    while (regex_search(start, end, mat, reg)) {   // regex_search一次只提取出一个匹配结果
        indexs.push_back(mat[0].first - str.begin());
        lens.push_back(mat[0].length());
        start = mat[0].second;    // 需要设置start以跳过当前已经匹配过的部分
    }

    if (indexs.empty()) {
        return str;
    }

#ifdef DEBUG
    cout << "[debug] == get $ var name ==" << endl;
    for (int i = 0; i < indexs.size(); ++i) {
        cout << '\t' << str.substr(indexs[i], lens[i]) << endl;
    }
    cout << "============================\n" << endl;
#endif

    string ret;
    ret += str.substr(0, indexs[0]);
    for (int i = 0; i < indexs.size(); ++i) {
        string name = str.substr(indexs[i] + 1, lens[i] - 1);
        char *var = getenv(name.c_str());
        if (var != nullptr) {
            ret += var;
        }
        if (i < indexs.size() - 1) {
            ret += str.substr(indexs[i] + lens[i], indexs[i + 1] - indexs[i] - lens[i]);
        }
    }
    ret += str.substr(indexs[indexs.size() - 1] + lens[lens.size() - 1]);

    return ret;
}

string basic_command::parse_word(const string &str) {
    string ret;

    // 去括号
    vector<int> start, end;
    int ret2 = find_quota_interval(str, start, end);
    if (ret2 == -1) {
        return str;
    }

    if (start.empty()) {
        return parse_var(str);
    }

    ret += parse_var(str.substr(0, start[0]));
    for (int i = 0; i < start.size(); ++i) {
        if (str[start[i]] == '\'') {
            // 单引号内容无需处理
            ret += str.substr(start[i] + 1, end[i] - start[i] - 2);
        } else if (str[start[i]] == '\"') {
            ret += parse_var(
                    str.substr(start[i] + 1, end[i] - start[i] - 2)
            );
        }
        if (i < start.size() - 1) {
            ret += parse_var(str.substr(end[i], start[i + 1] - end[i]));
        }
    }
    ret += parse_var(str.substr(end[end.size() - 1]));

    return ret;
}

/**
 * 解析字段中的  '  "  $
 */
int basic_command::parse_all() {
    // todo 首先找到当前所有的 environ


    // 取出所有的 env_name
    vector<string> env_name_list;
    for (auto &it:env_list) {
        env_name_list.push_back(it.first);
    }
    for (auto &it:env_name_list) {
        string new_name = parse_word(it);
        string new_cont = parse_word(env_list[it]);
        env_list.erase(it);
        env_list[new_name] = new_cont;
    }
    executable = parse_word(executable);
    for (auto &it : arg_list) {
        it = parse_word(it);
    }
    for (auto &it:io_list) {
        if (it.type != ">&" && it.type != "<&") {
            it.str = parse_word(it.str);
        }
    }
    return 0;
}

int basic_command::apply_io_mod() {
    for (auto &it:io_list) {
        it.apply();
    }
    return 0;
}


#endif