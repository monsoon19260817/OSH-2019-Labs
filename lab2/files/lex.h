#ifndef SHELL_LEX_H
#define SHELL_LEX_H

/**
 * 词法分析
 */

#include <string>
#include <set>
#include <vector>

//enum Lex_Type {
//    Word,
//    Key
//    Great,          // >
//    Great_Great,    // >>
//    Less,           // <
//    And_Great,      // &>
//    And_Great_Great,// &>>
//    Great_And,      // >&
//    Less_And,       // <&
//    Less_Less,      // <<
//    Less_Less_Less, // <<<
//    Pipe,           // |
//    And,            // &
//    Semicolon,      // ；
//    Left_Bracket,   // (
//    Right_Bracket   // )
//};

// 关键字符
set<char> key_char = {'>', '<', '&', '|', ';', '(', ')'};
// 分割字符
set<char> split_char = {' ', '\n', '\r', '\t', '\f', '\v'};
// 关键字
set <string> key_word = {
        ">", ">>", "<",
        ">&", "<&",
//        "<<", "<<<",
        "|", "&", ";",
        "(", ")"
};
// io 重定向关键字
set <string> io_key_word = {
        ">", ">>", "<",
        ">&", "<&",
//        "<<", "<<<"
};

/**
 * 词法分析函数声明
 */
vector<string> lex(string &str);

#endif
