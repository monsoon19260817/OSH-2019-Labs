#ifndef SHELL_PARSE_CPP
#define SHELL_PARSE_CPP

#include "utils.cpp"
#include "command.h"

/**
 * 语法分析
 * 返回三种命令中一种
 */
command *parse(vector<string> &words) {
    auto cmds = split_subcmd(words);
    if (cmds.size() == 1) {
        // 只有一个命令
        if (has_pipe(cmds[0])) {
            // 单一 pipe 命令
            bool is_bg = false;
            if (cmds[0].back() == "&") {
                is_bg = true;
                cmds[0].pop_back();
            }
            auto tmp = split_pipe(cmds[0]);
#ifdef DEBUG
            cout << "[debug] get pipe cmd\n" << endl;
#endif
            auto pcmd = new pipe_command(tmp);
            pcmd->is_bg = is_bg;
            return pcmd;
        } else if (has_bracket(cmds[0])) {
            if (check_bracket_single_cmd(cmds[0])) {
                auto tmp = remove_bracket(cmds[0]);
#ifdef DEBUG
                cout << "[debug] == remove bracket result ==" << endl;
                cout << '\t';
                for (auto &it:tmp) {
                    cout << it << '\t';
                }
                cout << "\n===================================\n" << endl;
#endif
                auto cmd = parse(tmp);
                cmd->is_bg = cmds[0][cmds[0].size() - 1] == "&";
                return cmd;
            } else {
                // 错误格式
                throw "syntax error near )";
            }
        } else {
#ifdef DEBUG
            cout << "[debug] get basic cmd\n" << endl;
#endif
            return new basic_command(cmds[0]);
        }
    } else {
        // 多个命令组合
#ifdef DEBUG
        cout << "[debug] get group cmd\n" << endl;
#endif
        return new group_command(cmds);
    }
}

#endif
