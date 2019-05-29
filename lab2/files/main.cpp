#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <readline/readline.h>
#include <readline/history.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>
#include <csignal>
#include <sys/utsname.h>

using namespace std;

//#define DEBUG

const int MAX_PATH_LEN = 2048;
//int bg_num=0;

#include "lex.h"
#include "lex.cpp"
#include "utils.cpp"
#include "parse.cpp"
#include "command.h"
#include "basic_command.cpp"
#include "group_command.cpp"
#include "pipe_command.cpp"

//void SIGINT_handler(int s) {
//    // 杀死所有子进程
//    signal(SIGCHLD, SIG_IGN);
//    kill(getpid(), SIGINT);
//    main();
//}



int main() {
    // todo SIGINT 接收器

#ifdef DEBUG
    int i = 0;
    cout << "[debug] enviro print" << endl;
    while (environ[i] != nullptr) {
        cout << '\t' << environ[i] << endl;
        i++;
    }
    cout << endl;
#endif
    struct utsname uts{};
    uname(&uts);
    for (;;) {
//        bg_num=0;
        string hint;
        hint += '[';
        hint += getenv("USER");
        hint += '@';
        hint += uts.nodename;
        if (getuid() == 0) {
            hint += "]# ";
        } else {
            hint += "]$ ";
        }
        string line = readline(hint.c_str());
        if (regex_match(line, regex("^\\s*$"))) {
            continue;
        }
        try {
            auto words = lex(line);
            auto cmd = parse(words);
            cmd->exec();
            delete cmd;
        } catch (const char *e) {
            cerr << e << endl;
            continue;
        }
    }
    return 0;
}