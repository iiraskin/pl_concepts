DEF_CMD(PUSHR, 8, "push", PushRHandling(i), PushRHandling(int &i), ,
    {fprintf(fd, "push ");
    CHECK_BUFF_END("CConverter::PrintWords: Incorrect command push (no argument)\n")

    if (PrintRegister(comm_buff, fd, i)) {
        PARSER_ERROR("CConverter::PrintWords: Error in CRPNHandler::PrintRegister\n")
    }
    fprintf(fd, "\n");})
