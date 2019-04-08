#pragma once

#include <cmath>

#include "NVM.h"
#include "Handling.cpp"

const char CHAR_INIT = 20;

int CNVM::Work()
{
    if (ReadBinFile() || Handling()) {
        printf("CNVM::Work: Error in another method\n");
        return 1;
    }
    delete[] buffer;
}

int CNVM::ReadBinFile()
{
    char com_file[256];
    printf("Input path to file with commands\n");
    scanf("%s", com_file);
    int fd = open(com_file, O_RDONLY|O_BINARY);
    struct stat buff = {};

    if (fstat(fd, &buff)) {
        printf("CNVM::ReadBinFile: Path to file is incorrect\n");
        return 1;
    }

    buffer = new char[buff.st_size + 1]{CHAR_INIT};
    buff_len = read(fd, buffer, buff.st_size);
    buffer[buff_len] = '\0';
    close(fd);

    comms_len = 0;
    for (int i = 0; i < MAXSIZELEN; ++i) {
        comms_len = comms_len * BYTESIZE + buffer[i];
    }
    stack_top = comms_len + 7;
    func_stack_top = buff_len;

    return 0;
}


int CNVM::StackPush(int element)
{
    if (func_stack_top <= stack_top) {
        printf("CNVM::StackPush: Stack is full\n");
        return 1;
    }
    ++stack_top;
    buffer[stack_top] = element;
    return 0;
}

int CNVM::StackPop(int& element)
{
    if (stack_top == comms_len + 7) {
        printf("CNVM::StackPop: Stack is empty\n");
        return 1;
    }
    element = buffer[stack_top];
    --stack_top;
    return 0;
}


int CNVM::RegisterPush(int reg)
{
    if (StackPush(buffer[comms_len + reg])) {
        printf("CNVM::RegisterPop: Error in CNVM::StackPush\n");
        return 1;
    }
    return 0;
}

int CNVM::RegisterPop(int reg)
{
    int element;
    if (StackPop(element)) {
        printf("CNVM::RegisterPush: Error in CNVM::StackPush\n");
        return 1;
    }
    buffer[comms_len + reg] = element;
    return 0;
}

int CNVM::FuncPush(int ret_addr)
{
    if (func_stack_top <= stack_top) {
        printf("CNVM::FuncPush: Functions stack is full\n");
        return 1;
    }
    --func_stack_top;
    buffer[func_stack_top] = ret_addr;
    return 0;
}

int CNVM::FuncPop(int& ret_addr)
{
    if (func_stack_top == buff_len) {
        printf("CNVM::FuncPop: Stack is empty\n");
        return 1;
    }
    ret_addr = buffer[func_stack_top];
    ++func_stack_top;
    return 0;
}
