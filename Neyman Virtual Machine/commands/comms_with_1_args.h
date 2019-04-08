DEF_CMD(JUMP, 14, "jump", JumpHandling(i, buffer[i]), JumpHandling(int &i, int cmd),
    {comm_buff.push_back(CMD_JUMP);

    std::string lab = {};

    if (ParseLabel(i, commands, commands_len, lab)) {
        PARSER_ERROR("CConverter::FromAssembler: Error in CConverter::ReadLabel\n")
    }

    if (labels.find(lab) == labels.end()) {
        std::vector<int> lab_pos = {comm_buff.size()};
        labels.insert({lab, lab_pos});
    } else {
        labels[lab].push_back(comm_buff.size());
    }
    comm_buff.push_back(0);},

    {CHECK_BUFF_END("CConverter::PrintWords: Incorrect command jump (no argument)\n")
    fprintf(fd, "jump %d\n", comm_buff[++i]);})

DEF_CMD(PUSH, 7, "push", PushHandling(i), PushHandling(int& i),
    {SKIP_SPACES

    CHECK_COMMS_END("CConverter::FromAssembler: Command 'push' does not has an argument\n")

    if (commands[i] == 'r') {
        comm_buff.push_back(CMD_PUSHR);
        if (ParseRegister(i, commands, commands_len, comm_buff)) {
            PARSER_ERROR("CConverter::FromAssembler: Error in CConverter::ParseRegister\n")
        }
    } else {
        comm_buff.push_back(CMD_PUSH);
        if (ParseNumber(i, commands, commands_len, comm_buff)) {
            PARSER_ERROR("CConverter::FromAssembler: Error in CConverter::ParseNumber\n")
        }
    }},

    {CHECK_BUFF_END("CConverter::PrintWords: Incorrect command push (no argument)\n")
    fprintf(fd, "push %d\n", comm_buff[++i]);})

DEF_CMD(POPR, 9, "pop", PopRHandling(i), PopRHandling(int& i),
    {SKIP_SPACES
    CHECK_COMMS_END("CConverter::FromAssembler: Command 'push' does not has an argument\n")

    if (commands[i] == 'r') {
        comm_buff.push_back(CMD_POPR);
        if (ParseRegister(i, commands, commands_len, comm_buff)) {
            PARSER_ERROR("CConverter::FromAssembler: Error in CConverter::ParseRegister\n")
        }
    } else {
        PARSER_ERROR("CConverter::FromAssembler: Incorrect argument\n")
    }},

    {fprintf(fd, "pop ");
    CHECK_BUFF_END("CConverter::PrintWords: Incorrect command pop (no argument)\n")

    if (PrintRegister(comm_buff, fd, i)) {
        PARSER_ERROR("CConverter::PrintWords: Error in CConverter::PrintRegister\n")
    }
    fprintf(fd, "\n");})

DEF_CMD(CALL, 21, "call", CallHandling(i), CallHandling(int& i),
    {comm_buff.push_back(CMD_CALL);
    std::string lab = {};

    if (ParseLabel(i, commands, commands_len, lab)) {
        PARSER_ERROR("CConverter::FromAssembler: Error in CConverter::ReadLabel\n")
    }

    if (labels.find(lab) == labels.end()) {
        std::vector<int> lab_pos = {comm_buff.size()};
        labels.insert({lab, lab_pos});
    } else {
        labels[lab].push_back(comm_buff.size());
    }
    comm_buff.push_back(0);},

    {CHECK_BUFF_END("CConverter::PrintWords: Incorrect command call (no argument)\n")
    fprintf(fd, "call %d\n", comm_buff[++i]);})

DEF_CMD(PRINT, 25, "print", PrintHandling(i), PrintHandling(int& i),
    {comm_buff.push_back(CMD_PRINT);
    std::string phrase = {};

    if (ParseLabel(i, commands, commands_len, phrase)) {
        PARSER_ERROR("CConverter::FromAssembler: Error in CConverter::ReadLabel\n")
    }

    if (phrases.find(phrase) == phrases.end()) {
        std::vector<int> phrase_pos = {comm_buff.size()};
        std::string s = "";
        phrases.insert({phrase, std::make_pair(s, phrase_pos)});
    } else {
        phrases[phrase].second.push_back(comm_buff.size());
    }
    comm_buff.push_back(0);},

    {CHECK_BUFF_END("CConverter::PrintWords: Incorrect command call (no argument)\n")
    fprintf(fd, "print %d\n", comm_buff[++i]);})
