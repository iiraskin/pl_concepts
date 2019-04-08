DEF_CMD(LABEL, 13, "label", , ,
    {comm_buff.push_back(CMD_LABEL);
    std::string lab = {};

    if (ParseLabel(i, commands, commands_len, lab)) {
        PARSER_ERROR("CConverter::FromAssembler: Error in CConverter::ReadLabel\n")
    }

    if (labels.find(lab) == labels.end()) {
        std::vector<int> lab_pos = {MAXSIZELEN + comm_buff.size()};
        labels.insert({lab, lab_pos});
    } else {
        labels[lab].insert(labels[lab].begin(), MAXSIZELEN + comm_buff.size());
    }},

    {CHECK_BUFF_END("CConverter::PrintWords: Incorrect command label (no argument)\n")
    fprintf(fd, "label %d\n", i + 1);})
