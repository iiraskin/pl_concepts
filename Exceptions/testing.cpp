#pragma once

#include "exceptions.h"

using std::cout;
using std::endl;

class A
{
public:
    A()
    {
        cout << "A::A" << endl;
    }

    void Deleting()
    {
        ds.deleted_classes.push(std::make_pair(this, [](const void* x) {static_cast<const A*>(x)->~A();}));
    }

    ~A()
    {
        cout << "A::~A" << endl;
    }
};

class B
{
public:
    B() {
        cout << "B::B. This class should not be deleted." << endl;
    }

    ~B() {
        cout << "B::~B. If you see this, something going wrong." << endl;
    }
};

class ClassWithException
{
public:
    ClassWithException()
    {
        cout << "CWE::CWE" << endl;
    }

    void Deleting()
    {
        ds.deleted_classes.push(std::make_pair(this, [](const void* x) {static_cast<const ClassWithException*>(x)->~ClassWithException();}));
    }

    ~ClassWithException()
    {
        cout << "CWE::~CWE. Now the DIVISION_BY_ZERO_EXCEPTION exception will try to be called." << endl;
        THROW(DIVISION_BY_ZERO_EXCEPTION)
    }

};

//Simple
void Test1()
{
    cout << "Test1:" << endl;
    cout << "OUT_OF_MEMORY_EXCEPTION should be caught." << endl;
    TRY ({
        A a1;
        a1.Deleting();
        A a2;
        a2.Deleting();
        THROW(OUT_OF_MEMORY_EXCEPTION)
    })
    CATCH(OUT_OF_MEMORY_EXCEPTION, {cout << "OUT_OF_MEMORY_EXCEPTION" << endl;})
}

//With class that should not be deleted.
void Test2()
{
    cout << "Test2:" << endl;
    cout << "OUT_OF_MEMORY_EXCEPTION should be caught." << endl;
    TRY ({
        A a1;
        a1.Deleting();
        B b;
        THROW(OUT_OF_MEMORY_EXCEPTION)
    })
    CATCH(OUT_OF_MEMORY_EXCEPTION, {cout << "OUT_OF_MEMORY_EXCEPTION" << endl;})
}

//With two nested functions.
void Test3()
{
    cout << "Test3:" << endl;
    cout << "OUT_OF_MEMORY_EXCEPTION should be caught by the Test1." << endl;
    TRY ({
        A a1;
        a1.Deleting();
        A a2;
        a2.Deleting();
        Test1();
        THROW(DIVISION_BY_ZERO_EXCEPTION)
    })
    CATCH(OUT_OF_MEMORY_EXCEPTION, {cout << "OUT_OF_MEMORY_EXCEPTION" << endl;})
}

//With an attempt to use throw in destructor.
void Test4()
{
    cout << "Test4:" << endl;
    cout << "Error of stack deleting should be occured." << endl;
    TRY ({
        A a1;
        a1.Deleting();
        ClassWithException cwe;
        cwe.Deleting();
        THROW(OUT_OF_MEMORY_EXCEPTION)
    })
    CATCH(OUT_OF_MEMORY_EXCEPTION, {cout << "OUT_OF_MEMORY_EXCEPTION" << endl;})
}
