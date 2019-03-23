#pragma once
#include <cmath>

#include "NVM.h"

#define HANDLING_ERROR(...) \
printf(__VA_ARGS__); \
return 1;

int CNVM::Handling()
{
    int i = 4;
    for (i; i < comms_len && !(buffer[i] == CMD_BEGIN &&
                               (i == 4 || buffer[i - 1] != CMD_PUSH && buffer[i - 1] != CMD_CALL &&
                                #define DEF_CMD(name, n, str, cond) \
                                buffer[i - 1] != CMD_##name &&
                                #include "commands/comms_jump.h"
                                #undef DEF_CMD
                                buffer[i - 1] != CMD_JUMP)); ++i) {}
    ++i;

    for (i; i < comms_len; ++i)
    {
        switch (buffer[i]) {
        #define DEF_CMD(name, n, str, cond) \
        case CMD_##name:
        #include "commands/comms_jump.h"
        #undef DEF_CMD

        #define DEF_CMD(name, n, str, func_title, func_with_args, parser, printer) \
        case CMD_##name: \
            if ( func_title ) { \
                HANDLING_ERROR("CNVM::Handling: Error in command handling function\n") \
            } \
            break;
        #include "commands/comms_with_1_args.h"
        #include "commands/comms_with_1_args_without_word.h"
        #include "commands/comms_with_0_args.h"
        #include "commands/comms_with_2_args.h"
        #undef DEF_CMD
        case CMD_BEGIN:
            HANDLING_ERROR("CNVM::Handling: Error: unexpected command 'begin'\n")
        case CMD_END:
            return 0;
        case CMD_LABEL:
            HANDLING_ERROR("CNVM::Handling: Error: unexpected command 'label'\n")
        default:
            HANDLING_ERROR("CNVM::Handling: Error: command %d does not exist\n", buffer[i]);
        }
    }

    HANDLING_ERROR("CNVM::Handling: Error: command 'begin' or 'end' is missed\n")
}

int CNVM::Arg_for_cmp(int& i, int& var) {
    if (i == comms_len - 1) {
        HANDLING_ERROR("CNVM::PopRHandling: Error: command 'push' does not has an argument\n")
    }
    switch (buffer[++i]) {
        #define DEF_R(name, n, str) \
        case R##name: \
            var = buffer[comms_len + R##name]; \
            break;
        #include "commands/registers.h"
        #undef DEF_R
        default:
            HANDLING_ERROR("CNVM::PopRHandling: Error: push argument %d does not exist\n", buffer[i])
    }
    return 0;
}

int CNVM::AddHandling()
{
    int a = 0;
    int b = 0;
    if (StackPop(a)) {
        HANDLING_ERROR("CNVM::AddHandling: Error: perhaps stack is empty (first argument)\n")
    }
    if (StackPop(b)) {
        HANDLING_ERROR("CNVM::AddHandling: Error: perhaps stack is empty (second argument)\n")
    }
    return StackPush(a + b);
}

int CNVM::SubHandling()
{
    int a = 0, b = 0;
    if (StackPop(a)) {
        HANDLING_ERROR("CNVM::SubHandling: Error: perhaps stack is empty (first argument)\n")
    }
    if (StackPop(b)) {
        HANDLING_ERROR("CNVM::SubHandling: Error: perhaps stack is empty (second argument)\n")
    }
    return StackPush(b - a);
}

int CNVM::MulHandling()
{
    int a = 0, b = 0;
    if (StackPop(a)) {
        HANDLING_ERROR("CNVM::MulHandling: Error: perhaps stack is empty (first argument)\n")
    }
    if (StackPop(b)) {
        HANDLING_ERROR("CNVM::MulHandling: Error: perhaps stack is empty (second argument)\n")
    }
    return StackPush(a * b);
}

int CNVM::DivHandling()
{
    int a = 0, b = 0;
    if (StackPop(a)) {
        HANDLING_ERROR("CNVM::DivHandling: Error: perhaps stack is empty (first argument)\n")
    }
    if (StackPop(b)) {
        HANDLING_ERROR("CNVM::DivHandling: Error: perhaps stack is empty (second argument)\n")
    }
    if (a == 0) {
        HANDLING_ERROR("CNVM::DivHandling: Error: divider is 0\n")
    }
    return StackPush(b / a);
}

int CNVM::SqrHandling()
{
    int a = 0;
    if (StackPop(a)) {
        HANDLING_ERROR("CNVM::SqrHandling: Error: perhaps stack is empty\n")
    }
    if (a < 0) {
        HANDLING_ERROR("CNVM::SqrHandling: Error: argument is below 0\n")
    }
    return StackPush(round(sqrt(a)));
}

int CNVM::InHandling()
{
    int a = 0;
    scanf("%d", &a);
    return StackPush(a);
}

int CNVM::OutHandling()
{
    int a = 0;
    if (StackPop(a)) {
        HANDLING_ERROR("CNVM::OutHandling: Error: perhaps stack is empty\n")
    }
    printf("%d\n", a);
    return 0;
}

int CNVM::PushHandling(int& i)
{
    if (i == comms_len - 1) {
        HANDLING_ERROR("CNVM::PushHandling: Error: command 'push' does not has an argument\n")
    }
    return StackPush(buffer[++i]);
}

int CNVM::PushRHandling(int& i)
{
    if (i == comms_len - 1) {
        HANDLING_ERROR("CNVM::PushRHandling: Error: command 'push' does not has an argument\n")
    }
    switch (buffer[++i]) {
    #define DEF_R(name, n, str) \
    case R##name: \
        if (RegisterPush(R##name)) { \
            HANDLING_ERROR("CNVM::PopRHandling: Error in CNVM::RegisterPop\n")\
        }\
        break;
    #include "commands/registers.h"
    #undef DEF_R
    default:
        HANDLING_ERROR("CNVM::PushRHandling: Error: push argument %d does not exist\n", buffer[i])
    }
    return 0;
}

int CNVM::PopRHandling(int& i)
{
    if (i == comms_len - 1) {
        HANDLING_ERROR("CNVM::PopRHandling: Error: command 'push' does not has an argument\n")
    }
    switch (buffer[++i]) {
    #define DEF_R(name, n, str) \
    case R##name: \
        if (RegisterPop(R##name)) { \
            HANDLING_ERROR("CNVM::PopRHandling: Error in CNVM::RegisterPop\n")\
        }\
        break;
    #include "commands/registers.h"
    #undef DEF_R
    default:
        HANDLING_ERROR("CNVM::PopRHandling: Error: push argument %d does not exist\n", buffer[i])
    }
    return 0;
}

int CNVM::CmpHandling(int& i)
{
    int a = 0, b = 0;

    if (Arg_for_cmp(i, a) || Arg_for_cmp(i, b)) {
        return 1;
    }

    if (a < b) {
        cmp_res = LESS;
    } else if (a > b) {
        cmp_res = MORE;
    } else {
        cmp_res = EQUAL;
    }
    return 0;
}

int CNVM::JumpHandling(int& i, int cmd)
{
    if (cmd != CMD_JUMP && cmp_res == UNDEFINED) {
        HANDLING_ERROR("CNVM::JumpHandling: Error: cmp has not been used yet\n")
    }

    ++i;
    switch (cmd) {
    #define DEF_CMD(name, n, str, cond) \
    case CMD_##name: \
        if (cmp_res cond) { \
            return 0; \
        } \
        break;
    #include "commands/comms_jump.h"
    #undef DEF_CMD
    }

    int lab = int(buffer[i]);

    if (lab < MAXSIZELEN || lab >= comms_len) {
        HANDLING_ERROR("CNVM::JumpHandling: This label is incorrect: %d\n", lab)
    }

    i = lab - 1;
    return 0;
}

int CNVM::CallHandling(int& i)
{
    int lab = int(buffer[++i]);

    if (lab < MAXSIZELEN || lab >= comms_len) {
        HANDLING_ERROR("CNVM::CallHandling: This label is is incorrect: %d\n", lab)
    }

    if (FuncPush(i)) {
        HANDLING_ERROR("CNVM::CallHandling: Error in CStack.Push(): %d\n", lab)
    }
    i = lab - 1;
    return 0;
}

int CNVM::RetHandling(int& i)
{
    if (FuncPop(i)) {
        HANDLING_ERROR("CNVM::RetHandling: Error in CStack.Pop()\nMaybe the stack is empty because program is not in function\n")
    }
    return 0;
}

int CNVM::PrintHandling(int& i)
{
    int phrase_addr = int(buffer[++i]);

    if (phrase_addr < MAXSIZELEN || phrase_addr >= comms_len || buffer[phrase_addr] != '"') {
        HANDLING_ERROR("CNVM::PhraseHandling: This phrase is incorrect: %d\n", phrase_addr)
    }

    for (int j = phrase_addr + 1; buffer[j] != '"'; ++j) {
        printf("%c", buffer[j]);
    }
    printf("\n");
    return 0;
}
