#ifndef UDC_DRIVER_HPP_
#define UDC_DRIVER_HPP_

#include <iostream>

#include "Location.hpp"
#include "Parser.hpp"
#include "Scanner.hpp"

#include "ast/All.hpp"

namespace udc {

class Driver {
public:
    Driver();

public:
    int Parse();
    void PrintSymbols();
    void SetProgram(std::unique_ptr<ast::Program> &&upProg) noexcept;

public:
    template<class ...tArgs>
    static inline void PrintError(const Location &vLoc, tArgs &&...vArgs) {
        ((std::cerr << "At " << vLoc << ":   error: ") << ... << std::forward<tArgs>(vArgs)) << std::endl;
    }

    template<class ...tArgs>
    static inline void PrintWarning(const Location &vLoc, tArgs &&...vArgs) {
        ((std::cerr << "At " << vLoc << ": warning: ") << ... << std::forward<tArgs>(vArgs)) << std::endl;
    }

    template<class ...tArgs>
    static inline void PrintNote(const Location &vLoc, tArgs &&...vArgs) {
        ((std::cerr << "At " << vLoc << ":    note: ") << ... << std::forward<tArgs>(vArgs)) << std::endl;
    }

private:
    Scanner x_vScanner;
    Parser x_vParser;
    std::unique_ptr<ast::Program> x_upProg;
};

}

#endif
