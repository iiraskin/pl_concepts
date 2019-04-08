#pragma once

#include <set>
#include "Converter.h"

#define CHECK_BUFF_END(...) \
if (i == comms_len - 1) { \
    PARSER_ERROR(__VA_ARGS__) \
}

int CConverter::Deconvert()
{
    char* comm_buff = {};

    char com_file[256];
    printf("Input path to file with commands\n");
    scanf("%s", com_file);
    int fd = open(com_file, O_RDONLY|O_BINARY);
    struct stat buff = {};

    if (fstat(fd, &buff)) {
        printf("CConverter::Convert: Path to file is incorrect\n");
        return 1;
    }

    comm_buff = new char[buff.st_size + 1]{CHAR_INIT};
    int comm_buff_len = read(fd, comm_buff, buff.st_size);
    comm_buff[comm_buff_len] = '\0';
    close(fd);

    int comms_len = 0;
    for (int i = 0; i < MAXSIZELEN; ++i) {
        comms_len = comms_len * BYTESIZE + comm_buff[i];
    }

    ToAssembler(comm_buff, comms_len);

    delete[] comm_buff;
    return 0;
}

int CConverter::ToAssembler(const char* comm_buff, int comms_len)
{
    char out_file[256];
    printf("Input path to output file\n");
    scanf("%s", &out_file);
    FILE* fd = fopen(out_file, "w");

    if (fd == nullptr) {
        PARSER_ERROR("CConverter::ToAssembler: Cannot open file for write\n")
    }

    bool is_end_achieved = false;

    for (int i = MAXSIZELEN; i < comms_len; ++i)
    {
        if (!is_end_achieved) {
            switch (comm_buff[i]){
            #define DEF_CMD(name, n, str, func_title, func_with_args, parser, printer) \
            case CMD_##name: \
                printer \
                break;
            #include "commands/comms_with_0_args.h"
            #include "commands/comms_with_1_args.h"
            #include "commands/comms_with_1_args_without_word.h"
            #include "commands/comms_with_2_args.h"
            #include "commands/comms_spec_with_0_args.h"
            #include "commands/comms_spec_with_1_args.h"
            #undef DEF_CMD

            #define DEF_CMD(name, n, str, cond) \
            case CMD_##name: \
                CHECK_BUFF_END("CConverter::ToAssembler: Incorrect command jump (no argument)\n") \
                fprintf(fd, "%s %d\n", str, comm_buff[++i]); \
                break;
            #include "commands/comms_jump.h"
            #undef DEF_CMD
            default:
                PARSER_ERROR("CConverter::ToAssembler: Error: command %d does not exist\n", comm_buff[i])
            }
        } else {
            if (comm_buff[i] != '"') {
                PARSER_ERROR("CConverter::ToAssembler: Error: command %d is after 'end'\n", comm_buff[i])
            }
            fprintf(fd, "%d %c", i, comm_buff[i]);
            ++i;
            for (i; comm_buff[i] != '"'; ++i) {
                fprintf(fd, "%c", comm_buff[i]);
            }
            fprintf(fd, "\"\n");
        }
    }

    fclose(fd);
    return 0;
}

int CConverter::PrintRegister(const char* comm_buff, FILE* fd, int& i)
{
    switch (comm_buff[++i]) {
    #define DEF_R(name, n, str) \
    case R##name: \
        fprintf(fd, "%s ", str); \
        break;
    #include "commands/registers.h"
    #undef DEF_R
    default:
        PARSER_ERROR("CRPNHandler::PrintWords: Error: pop argument %d does not exist\n", comm_buff[i])
    }
    return 0;
}
