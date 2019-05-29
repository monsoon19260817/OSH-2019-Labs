#ifndef SHELL_UTILS_CPP
#define SHELL_UTILS_CPP

/**
 * 识别 ;  &
 * 并据此划分子命令
 * 注意 ()
 */
vector<vector<string>> split_subcmd(vector<string> &cmd) {
    vector<int> indexs;
    vector<vector<string>> cmds;
    int depth = 0;
    // 找到所有分割符
    for (int i = 0; i < cmd.size(); ++i) {
        if (cmd[i] == "(") {
            depth++;
        } else if (cmd[i] == ")") {
            depth--;
        }
        if (depth == 0 && (cmd[i] == ";" || cmd[i] == "&")) {
            indexs.push_back(i);
        }
    }
    if (depth != 0) {
        // 括号未闭合
        throw "syntax error: bracket not closed";
    }
    if (!indexs.empty() && indexs[indexs.size() - 1] != cmd.size() - 1) {
        // 结尾没有 ； & 则添上 ;
        cmd.push_back(";");
        indexs.push_back(cmd.size() - 1);
    }
    if (indexs.empty()) {
        cmds.push_back(cmd);
    } else {
        for (int i = 0; i < indexs.size(); ++i) {
            int start = i == 0 ? 0 : (indexs[i - 1] + 1);
            int end = cmd[indexs[i]] == "&" ? (indexs[i] + 1) : indexs[i];
            vector<string> subcmd;
            for (int j = start; j < end; ++j) {
                subcmd.push_back(cmd[j]);
            }
            cmds.push_back(subcmd);
        }
//        int start = indexs[indexs.size() - 1] + 1;
//        int end = cmd.size();
//        vector<string> subcmd;
//        for (int j = start; j < end; ++j) {
//            subcmd.push_back(cmd[j]);
//        }
//        cmds.push_back(subcmd);
    }
#ifdef DEBUG
    cout << "[debug] === split sub cmd result ===" << endl;
    for (int i = 0; i < cmds.size(); i++) {
        cout << "\t== sub cmd " << (i + 1) << " ==" << endl;
        cout << '\t';
        for (auto &it:cmds[i]) {
            cout << it << "    ";
        }
        cout << endl;
    }
    cout << "[debug] ============================\n" << endl;
#endif
    return cmds;
}

/**
 * 传入非组命令
 * 划分 pipe
 * 如果不是 pipe 命令 返回 null
 */
vector<vector<string>> split_pipe(vector<string> &cmd) {
    vector<int> indexs;
    vector<vector<string>> cmds;
    int depth = 0;
    // 找到所有 pipe
    for (int i = 0; i < cmd.size(); ++i) {
        if (cmd[i] == "(") {
            depth++;
        } else if (cmd[i] == ")") {
            depth--;
        }
        if (depth == 0 && cmd[i] == "|") {
            indexs.push_back(i);
        }
    }
    if (depth != 0) {
        // 括号未闭合
        throw "syntax error: bracket not closed";
    }
    if (indexs.empty()) {
        throw "syntax error";
    } else {
        for (int i = 0; i < indexs.size(); ++i) {
            int start = i == 0 ? 0 : (indexs[i - 1] + 1);
            int end = indexs[i];
            vector<string> subcmd;
            for (int j = start; j < end; ++j) {
                subcmd.push_back(cmd[j]);
            }
            cmds.push_back(subcmd);
        }
//        int start = indexs[indexs.size() - 1] + 1;
//        int end = cmd.size();
//        vector<string> subcmd;
//        for (int j = start; j < end; ++j) {
//            subcmd.push_back(cmd[j]);
//        }
//        cmds.push_back(subcmd);
    }
    // 补全最后一个 pipe 节
    if (indexs[indexs.size() - 1] == cmd.size() - 1) {
        // 最后一个是 |
        throw "syntax error near |";
    }
    vector<string> subcmd;
    for (int j = indexs[indexs.size() - 1] + 1; j < cmd.size(); ++j) {
        subcmd.push_back(cmd[j]);
    }
    cmds.push_back(subcmd);
#ifdef DEBUG
    cout << "[debug] === split pipe cmd result ===" << endl;
    for (int i = 0; i < cmds.size(); i++) {
        cout << "\t== sub cmd " << (i + 1) << " ==" << endl;
        cout << '\t';
        for (auto &it:cmds[i]) {
            cout << it << "    ";
        }
        cout << endl;
    }
    cout << "[debug] ============================\n" << endl;
#endif
    return cmds;
}

/**
 * 传入非 pipe 命令，非组合命令
 * 检测是否有括号
 */
bool has_bracket(vector<string> &cmd) {
    for (auto &it:cmd) {
        if (it == "(" || it == ")") {
            return true;
        }
    }
    return false;
}

/**
 * 传入非组合命令，检查是否有 pipe
 */
bool has_pipe(vector<string> &cmd) {
    for (auto &it:cmd) {
        if (it == "|") {
            return true;
        }
    }
    return false;
}

/**
 * 检查字符串中是否有 \s
 */
bool has_splitor(string &str) {
    for (char c:split_char) {
        if (str.find(c) != -1) {
            return true;
        }
    }
    return false;
}

/**
 * 检查带括号的非组合，非 pipe 命令合法性
 * 只能是 ( ... ) [&]
 */
bool check_bracket_single_cmd(vector<string> &cmd) {
    if (cmd.size() < 2) {
        return false;
    }
    if (cmd[cmd.size() - 1] == "&") {
        return cmd[0] == "(" && cmd[cmd.size() - 2] == ")";
    } else {
        return cmd[0] == "(" && cmd[cmd.size() - 1] == ")";
    }
}

/**
 * 传入 ( ... ) [&] 的命令
 * 去除括号
 */
vector<string> remove_bracket(vector<string> &cmd) {
    vector<string> ret;
    int end = cmd[cmd.size() - 1] == "&" ? cmd.size() - 2 : cmd.size() - 1;
    for (int i = 1; i < end; ++i) {
        ret.push_back(cmd[i]);
    }
    return ret;
}

/**
 * 检查是否是基本命令
 * 不能有 ( ) |  ;
 */
bool is_basic_cmd(vector<string> &cmd) {
    for (auto &it:cmd) {
        if (it == "|" || it == "(" || it == ")" || it == ";") {
            return false;
        }
    }
    return true;
}

/**
 * 找到基本命令中属于 envset 的等号
 */
vector<int> find_all_envset_equal(vector<string> &cmd) {
    vector<int> ret;
    for (int i = 0; i < cmd.size(); ++i) {
        if (cmd[i] == "=") {
            ret.push_back(i);
        }
    }
    // 等号位置在 1 4 7 10  3i+1
    for (int i = 0; i < ret.size(); ++i) {
        if (ret[i] != 3 * i + 1) {
            // 不属于 envset 的等号
            // size() - i 个
            int num = ret.size() - i;
            for (int j = 0; j < num; ++j) {
                ret.pop_back();
            }
            break;
        }
    }
    return ret;
}

vector<int> find_all_io_key_word(vector<string> &cmd, const int start) {
    vector<int> ret;
    for (int i = start; i < cmd.size(); ++i) {
        if (is_io_key_word(cmd[i])) {
            ret.push_back(i);
        }
    }
    return ret;
}

int find_eq(string &str) {
    bool in_quota = false;
    char quota_char;
    for (int i = 0; i < str.size(); ++i) {
        if (in_quota) {
            if (str[i] == quota_char) {
                in_quota = false;
            }
        } else {
            if (str[i] == '\'' || str[i] == '\"') {
                in_quota = true;
                quota_char = str[i];
            } else if (str[i] == '=') {
                return i;
            }
        }
    }
    return -1;
}

bool is_signed_number(string &str) {
    if (!str.empty()) {
        if (str[0] == '-' || str[0] == '+') {
            auto tmp = str.substr(1);
            return is_number(tmp);
        } else {
            return is_number(str);
        }
    }
    return false;
}

#endif