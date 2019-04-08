#pragma once

#include <vector>
#include <unordered_map>

#define PARSER_ERROR(...) \
printf(__VA_ARGS__); \
return 1;

class CConverter
{
private:
    const char CHAR_INIT = 20;
    const char spaces[4] = {' ', '\n', '\t', '\r'};
    const int ZERO_CODE = '0';
    const int NINE_CODE = '9';
    const int MAXSIZELEN = 4;
    const int FILESIZECONSTANT = 100;
    const int BYTESIZE = 128;

private:
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

public:
    int Convert();
    int Deconvert();

private:
    int FromAssembler(const char* commands, const int commands_len, int& phrases_len, std::vector<int>& comm_buff,
                      std::vector<std::string>& phrases_to_print);

    int ToCode(int phrases_len, std::vector<int> comm_buff, std::vector<std::string> phrases_to_print);
    int ToAssembler(const char* comm_buff, int comms_len);

    bool IsSpace(char c) const;
    int ParseLabel(int& i, const char* commands, const int commands_len, std::string& lab);
    int ParsePhrase(int& i, const char* commands, const int commands_len, std::string& phrase);
    int ParseNumber(int& i, const char* commands, const int commands_len, std::vector<int>& comm_buff);
    int ParseRegister(int& i, const char* commands, const int commands_len, std::vector<int>& comm_buff);

    int PrintRegister(const char* comm_buff, FILE* fd, int& i);
};
