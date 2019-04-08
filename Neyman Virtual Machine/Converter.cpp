#pragma once

#include <unordered_map>
#include <io.h>
#include <sys\stat.h>
#include <fcntl.h>
#include <math.h>

#include "Converter.h"
#include "Deconvert.cpp"

#define SKIP_SPACES while (IsSpace(commands[i]) && i < commands_len) {++i;}

#define CHECK_COMMS_END(...) \
if (i == commands_len - 1) { \
    PARSER_ERROR(__VA_ARGS__) \
}

int CConverter::Convert()
{
    std::vector<int> comm_buff = {};
    std::vector<std::string> phrases_to_print = {};

    char com_file[256];
    printf("Input path to file with commands\n");
    scanf("%s", com_file);
    int fd = open(com_file, O_RDONLY);
    struct stat buff = {};

    if (fstat(fd, &buff)) {
        printf("CConverter::Convert: Path to file is incorrect\n");
        return 1;
    }

    char* commands = new char[buff.st_size + 1]{CHAR_INIT};
    int commands_len = read(fd, commands, buff.st_size);
    commands[commands_len] = '\0';
    close(fd);

    int phrases_len = 0;

    if (FromAssembler(commands, commands_len, phrases_len, comm_buff, phrases_to_print)) {
        printf("CConverter::Convert: Error in CConverter::FromAssembler\n");
        delete[] commands;
        return 1;
    }

    delete[] commands;

    ToCode(phrases_len, comm_buff, phrases_to_print);
    return 0;
}

int CConverter::ToCode(int phrases_len, std::vector<int> comm_buff, std::vector<std::string> phrases_to_print)
{
    char out_file[256];
    printf("Input path to output file\n");
    scanf("%s", &out_file);
    FILE* f = fopen(out_file, "wb");

    int code_size = MAXSIZELEN + comm_buff.size() + phrases_len;
    int code_size_copy = code_size;
    for (int i = MAXSIZELEN - 1; i >= 0; --i) {
        fprintf(f, "%c", static_cast<int>(code_size_copy / pow(BYTESIZE, i)));
        code_size_copy %= static_cast<int>(pow(BYTESIZE, i));
    }

    for (int i = 0; i < comm_buff.size(); ++i) {
        fprintf(f, "%c", comm_buff[i]);
    }

    for (phrase: phrases_to_print) {
        for (int j = 0; j < phrase.size(); ++j) {
            fprintf(f, "%c", phrase[j]);
        }
    }

    fclose(f);

    int g = open(out_file, O_WRONLY|O_BINARY);
    if (chsize(g, comm_buff.size() * FILESIZECONSTANT)) {
        printf("CConverter::Convert: Can't change size of file.\n");
        close(g);
        return 1;
    }
    close(g);
    return 0;
}

int CConverter::FromAssembler(const char* commands, const int commands_len, int& phrases_len, std::vector<int>& comm_buff,
                              std::vector<std::string>& phrases_to_print)
{
    comm_buff = {};
    std::unordered_map< std::string, std::vector<int> > labels = {};
    std::unordered_map< std::string, std::pair< std::string, std::vector<int> > > phrases = {};
    int i = 0;

    SKIP_SPACES

    while (i < commands_len) {

        std::string com = {};
        for (i; !IsSpace(commands[i]) && i < commands_len; ++i) {
            com += commands[i];
        }

        #define DEF_CMD(name, n, str, func_title, func_with_args, parser, printer) \
        if (com == str) { \
            parser \
        } else
        #include "commands/comms_with_0_args.h"
        #include "commands/comms_with_1_args.h"
        #include "commands/comms_with_2_args.h"
        #include "commands/comms_spec_with_0_args.h"
        #include "commands/comms_spec_with_1_args.h"
        #undef DEF_CMD

        #define DEF_CMD(name, n, str, cond) \
        if (com == str) { \
            comm_buff.push_back(CMD_##name); \
            std::string lab = {}; \
            if (ParseLabel(i, commands, commands_len, lab)) { \
                PARSER_ERROR("CConverter::FromAssembler: Error in CRPNHandler::ReadLabel\n") \
            } \
            if (labels.find(lab) == labels.end()) { \
                std::vector<int> lab_pos = {comm_buff.size()}; \
                labels.insert({lab, lab_pos}); \
            } else { \
                labels[lab].push_back(comm_buff.size());\
            } \
            comm_buff.push_back(0); \
        } else
        #include "commands/comms_jump.h"
        #undef DEF_CMD
        {
            std::string phrase = {};
            if (ParsePhrase(i, commands, commands_len, phrase)) {
                PARSER_ERROR("CConverter::FromAssembler: Error in CConverter::ReadPhrase\n")
            }

            if (phrases.find(com) == phrases.end()) {
                std::vector<int> phrase_pos = {MAXSIZELEN + comm_buff.size()};
                phrases.insert({com, std::make_pair(phrase, phrase_pos)});
            } else {
                phrases[com].first = phrase;
                phrases[com].second.insert(phrases[com].second.begin(), MAXSIZELEN + comm_buff.size());
            }
        }

        SKIP_SPACES
    }

    for (label: labels) {
        for (int j = 1; j < label.second.size(); ++j) {
            comm_buff[label.second[j]] = label.second[0];
        }
    }

    phrases_len = 0;
    for (phrase: phrases) {
        for (int j = 1; j < phrase.second.second.size(); ++j) {
            comm_buff[phrase.second.second[j]] = MAXSIZELEN + comm_buff.size() + phrases_len;
        }
        phrases_len += phrase.second.first.size();
        phrases_to_print.push_back(phrase.second.first);
    }

    return 0;
}

bool CConverter::IsSpace(char c) const
{
    for (space: spaces) {
        if (c == space) {
            return true;
        }
    }
    return false;
}

int CConverter::ParseLabel(int& i, const char* commands, const int commands_len, std::string& lab)
{
    SKIP_SPACES

    CHECK_COMMS_END("CConverter::ParseReadLabel: Command does not has an argument\n")

    for (i; !IsSpace(commands[i]) && i < commands_len; ++i) {
        lab += commands[i];
    }
    return 0;
}

int CConverter::ParsePhrase(int& i, const char* commands, const int commands_len, std::string& phrase)
{
    SKIP_SPACES

    CHECK_COMMS_END("CConverter::ParseReadLabel: Command does not has an argument\n")

    if (commands[i] != '"') {
        PARSER_ERROR("CConverter::ParsePhrase: \" was expected\n")
    }
    phrase = '"';
    ++i;

    for (i; commands[i] != '"' && i < commands_len; ++i) {
        phrase += commands[i];
    }
    if (i == commands_len) {
        PARSER_ERROR("CConverter::ParsePhrase: \" was expected at the end of a phrase\n")
    }
    phrase += '"';
    ++i;
    return 0;
}

int CConverter::ParseNumber(int& i, const char* commands, const int commands_len, std::vector<int>& comm_buff)
{
    SKIP_SPACES

    bool sign = false;
    if (commands[i] == '-') {
        sign = true;
        ++i;
    }

    CHECK_COMMS_END("CRPNHandler::ParseNumber: Command 'push' does not has an argument\n")

    int num = 0;
    for (i; !IsSpace(commands[i]) && commands[i] != ']' && i < commands_len; ++i) {
        int n = static_cast<int>(commands[i]);

        if (n < ZERO_CODE || n > NINE_CODE) {
            PARSER_ERROR("CRPNHandler::ParseNumber: Incorrect argument for command 'push' with %c\n", commands[i])
        }

        num = num * 10 + n - ZERO_CODE;
    }

    if (sign) {
        num = -num;
    }

    comm_buff.push_back(num);
    return 0;
}

int CConverter::ParseRegister(int& i, const char* commands, const int commands_len, std::vector<int>& comm_buff)
{
    SKIP_SPACES
    std::string r = {};
    for (i; !IsSpace(commands[i]) && i < commands_len; ++i) {
        r += commands[i];
    }
    #define DEF_R(name, n, str) \
    if (r == (str)) { \
        comm_buff.push_back(R##name); \
    } else
    #include "commands/registers.h"
    #undef DEF_R
    {
        PARSER_ERROR("CConverter::ParseRegister: Unknown argument %s\n", r)
    }
    return 0;
}
