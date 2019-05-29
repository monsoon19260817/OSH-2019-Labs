#ifndef SHELL_COMMAND_H
#define SHELL_COMMAND_H

enum CMD_TYPE {
    BASIC_CMD,
    PIPE_CMD,
    GROUP_CMD
};

class command {
public:
    CMD_TYPE type;

    virtual int exec() = 0;

    bool is_bg = false;
};

class io_mod {
public:
    string type;
    int num1, num2;
    string str;

    int apply();
};

class basic_command : public command {
public:
    explicit basic_command(vector<string> &str);

    int exec() override;

//    ~basic_command() override ;

private:
    map<string, string> env_list;
    string executable;
    vector<string> arg_list;
    vector<io_mod> io_list;

    int exec_builtin();

    int exec_cd();

    int exec_alias();

    int exec_export();

    int exec_exit();

    string parse_word(const string &str);

    int parse_all();

    int apply_io_mod();
};

class group_command : public command {
public:
    explicit group_command(vector<vector<string>> &str);

    int exec() override;

    ~group_command();

private:
    vector<command *> subcmd_list;
};

class pipe_command : public command {
public:
    explicit pipe_command(vector<vector<string>> &str);

    int exec() override;

    ~pipe_command();

private:
    vector<basic_command *> subcmd_list;
};

#endif
