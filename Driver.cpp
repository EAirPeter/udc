#include "Driver.hpp"

namespace udc {

Driver::Driver() : x_vScanner(*this, false), x_vParser(*this, x_vScanner) {}

int Driver::Parse() {
    auto nRes = x_vParser.parse();
    if (x_upProgram)
        std::cout << *x_upProgram << std::endl;
    return nRes;
}

void Driver::PrintSymbols() {
    x_vScanner.SetPrint(true);
    while (x_vScanner.NextSymbol().token() != TokenType::kEnd);
    x_vScanner.SetPrint(false);
}

void Driver::SetProgram(std::unique_ptr<ast::Program> &&upProgram) noexcept {
    x_upProgram = std::move(upProgram);
}

}
