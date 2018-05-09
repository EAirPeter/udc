#ifndef UDC_DRIVER_HPP_
#define UDC_DRIVER_HPP_

#include <iostream>

#include "Location.hpp"
#include "Parser.hpp"
#include "Scanner.hpp"

namespace udc {

class Driver {
public:
    Driver();

public:
    int Parse();

    void PrintSymbols();

public:
    template<class ...tArgs>
    static inline void PrintError(const Location &vLoc, tArgs &&...vArgs) {
        ((std::cerr << "At " << vLoc << ": ") << ... << std::forward<tArgs>(vArgs)) << std::endl;
    }

private:
    Scanner x_vScanner;
    Parser x_vParser;

};

}

#endif
