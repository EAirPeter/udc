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
    void SetProgram(ast::Program &upProgram) noexcept;

    template<class tNode, class ...tArgs>
    tNode &AddNode(tArgs &&...vArgs) {
        auto &&upNode = std::make_unique<tNode>(std::forward<tArgs>(vArgs)...);
        auto pNode = upNode.get();
        x_vecNodes.emplace_back(std::move(upNode));
        return *pNode;
    }

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
    std::vector<std::unique_ptr<ast::NodeBase>> x_vecNodes;
    std::optional<std::reference_wrapper<ast::Program>> x_oProg;

};

}

#endif
