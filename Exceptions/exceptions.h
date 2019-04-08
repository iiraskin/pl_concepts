#pragma once

#include <stdlib.h>
#include <setjmp.h>
#include <functional>
#include <utility>
#include <stack>

enum exceptions {BAD_FILE_EXCEPTION, OUT_OF_MEMORY_EXCEPTION, DIVISION_BY_ZERO_EXCEPTION, NULL_POINTER_EXCEPTION};

#define TRY(code) \
    int val = setjmp(ex_data.env); \
    if (!val) { \
        ds.deleted_classes.push(std::make_pair(nullptr, [](const void* arg){})); \
        code \
        ds.del(); \
    } \

#define CATCH(exc, code) \
    else if (val == exc) { \
        code \
    }

#define THROW(exc) \
    ds.del(); \
    ex_data.is_ex = false; \
    longjmp(ex_data.env, exc);

struct ExceptionData
{
    jmp_buf env;
    bool is_ex = false;
};

ExceptionData ex_data;

struct DeletingStack
{
    std::stack< std::pair< void*, std::function<void(const void* x)> > > deleted_classes;

    void del() {
        if (ex_data.is_ex) {
            std::cout << "DeletingStack::del: Error: re-exception during stack cleanup." << std::endl;
            exit(1);
        }
        while(!deleted_classes.empty()) {
            ex_data.is_ex = true;

            if (deleted_classes.top().first) {
                auto el = deleted_classes.top();
                deleted_classes.pop();
                el.second(el.first);

            } else {
                deleted_classes.pop();
                break;
            }
        }
    }
};

DeletingStack ds;
