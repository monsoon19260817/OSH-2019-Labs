#ifndef SHELL_LEX_CPP
#define SHELL_LEX_CPP

#include <vector>
#include <string>
#include <iostream>

#include "lex.h"

using namespace std;

bool is_split_char(char c) {
    return split_char.find(c) != split_char.end();
}

bool is_key_char(char c) {
    return key_char.find(c) != key_char.end();
}

bool is_key_word(string &str) {
    return key_word.find(str) != key_word.end();
}

bool is_io_key_word(string &str) {
    return io_key_word.find(str) != io_key_word.end();
}

bool is_number(string &str) {
    for (char c : str) {
        if (c < '0' || c > '9') {
            return false;
        }
    }
    return true;
}

/**
 * 词法分析函数
 */
vector<string> lex(string &str) {
    vector<string> lex_list;
    bool in_word = false;
    bool in_key = false;
    int start = 0;
    bool in_quota = false;
    char quota_char;
    for (int i = 0; i < str.size(); ++i) {
        char c = str[i];
        if (in_word) {
            if (in_quota) {
                // 结束引号
                if (c == quota_char) {
                    in_quota = false;
                }
            } else {
                if (c == '\'' || c == '\"') {
                    // 进入引号
                    in_quota = true;
                    quota_char = c;
                } else if (is_split_char(c)) {
                    // 发现分隔符
                    in_word = false;
                    lex_list.push_back(str.substr(start, i - start));
                } else if (is_key_char(c)) {
                    // 发现 key word
                    in_word = false;
                    lex_list.push_back(str.substr(start, i - start));
                    in_key = true;
                    start = i;
                }
            }
        } else if (in_key) {
            if (!is_key_char(c) ||
                str[start] == '(' || str[start] == ')' || str[start] == ';' ||
                str[start] == '|' || str[start] == '&') {
                // 结束 key
                in_key = false;
                auto key = str.substr(start, i - start);
                // 分析 key 是否合法
                if (is_key_word(key)) {
                    // 合法
                    lex_list.push_back(key);
                } else {
                    throw ("syntax error near " + key).c_str();
                }
                if (is_key_char(c)) {
                    // 进入 key word
                    in_key = true;
                    start = i;
                } else if (!is_split_char(c)) {
                    // 进入 word
                    in_word = true;
                    start = i;
                    if (c == '\'' || c == '\"') {
                        in_quota = true;
                        quota_char = c;
                    }
                }
            }
        } else {
            if (is_key_char(c)) {
                // 发现 key word
                in_key = true;
                start = i;
            } else if (!is_split_char(c)) {
                // 其他非分隔符的情况
                in_word = true;
                start = i;
                if (c == '\'' || c == '\"') {
                    in_quota = true;
                    quota_char = c;
                }
            }
        }
    }
    // 结束词法分析
    if (in_word) {
        if (in_quota) {
            // 引号未闭合
            throw (string("syntax error near ") + quota_char + ":bracket isn't closed").c_str();
        } else {
            lex_list.push_back(str.substr(start));
        }
    } else if (in_key) {
        auto key = str.substr(start);
        // 分析 key 是否合法
        if (is_key_word(key)) {
            // 合法
            lex_list.push_back(key);
        } else {
            // 非法 key
            throw ("syntax error near " + key).c_str();
        }
    }

#ifdef DEBUG
    cout << "[debug] ==== lex result ====" << endl;
    cout << '\t';
    for (auto &it:lex_list) {
        cout << it << "    ";
    }
    cout << "\n[debug] ====================\n" << endl;
#endif

    return lex_list;
}

#endif