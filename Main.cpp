#include "Driver.hpp"

#include <iostream>

int main() {
    udc::Driver vDriver;
    //vDriver.PrintSymbols();
    auto res = vDriver.Parse();
    std::cout << "Result: " << res << std::endl;
    return 0;
}
