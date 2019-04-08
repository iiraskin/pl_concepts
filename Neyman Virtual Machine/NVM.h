#pragma once

class CNVM
{
private:
    char* buffer;
    int buff_len;
    int comms_len;
    int stack_top;
    int cmp_res = UNDEFINED;
    int func_stack_top;
    const int MAXSIZELEN = 4;
    const int BYTESIZE = 128;

    enum commands {
        #define DEF_CMD(name, n, str, func_title, func_with_args, parser, printer) \
        CMD_##name = (n),
        #include "commands/comms_with_1_args.h"
        #include "commands/comms_with_1_args_without_word.h"
        #include "commands/comms_with_0_args.h"
        #include "commands/comms_with_2_args.h"
        #include "commands/comms_spec_with_0_args.h"
        #include "commands/comms_spec_with_1_args.h"
        #undef DEF_CMD

        #define DEF_CMD(name, n, str, cond) \
        CMD_##name = (n),
        #include "commands/comms_jump.h"
        #undef DEF_CMD
        DUMMY_CMD
    };

    enum registers {
        #define DEF_R(name, n, str) \
        R##name = (n),
        #include "commands/registers.h"
        #undef DEF_R
        DUMMY_R
    };

    enum compares {EQUAL, LESS, MORE, UNDEFINED};

public:
    int Work();

private:
    int ReadBinFile();

    int StackPush(int element);
    int StackPop(int& element);
    int RegisterPush(int reg);
    int RegisterPop(int reg);
    int FuncPush(int ret_addr);
    int FuncPop(int& ret_addr);

    int Handling();
    #define DEF_CMD(name, n, str, func_title, func_with_args, parser, printer) \
    int func_with_args;
    #include "commands/comms_with_1_args.h"
    #include "commands/comms_with_1_args_without_word.h"
    #include "commands/comms_with_0_args.h"
    #include "commands/comms_with_2_args.h"
    #undef DEF_CMD
    int Arg_for_cmp(int& i, int& var);
};
