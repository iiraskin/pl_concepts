DEF_CMD(CMP, 12, "cmp", CmpHandling(i), CmpHandling(int& i),
    {comm_buff.push_back(CMD_CMP);

    if (ParseRegister(i, commands, commands_len, comm_buff)) {
        PARSER_ERROR("CConverter::FromAssembler: Error in CConverter::ParseRegister\n")
    }
    if (ParseRegister(i, commands, commands_len, comm_buff)) {
        PARSER_ERROR("CConverter::FromAssembler: Error in CConverter::ParseRegister\n")
    }},

    {fprintf(fd, "cmp ");
    for (int j = 0; j < 2; ++j) {
        CHECK_BUFF_END("CConverter::PrintWords: Incorrect command pop (no argument)\n")

        if (PrintRegister(comm_buff, fd, i)) {
            PARSER_ERROR("CConverter::PrintWords: Error in CConverter::PrintRegister\n")
        }
    }
    fprintf(fd, "\n");})
