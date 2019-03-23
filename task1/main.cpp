#include <iostream>
#include <vector>

#include "Converter.cpp"
#include "NVM.cpp"

int main()
{
    while (true) {
        CConverter c;
        c.Convert();
        c.Deconvert();
        CNVM nvm;
        nvm.Work();
        std::cout << std::endl;
    }
    return 0;
}
