#include "Driver.hpp"

namespace udc {

Driver::Driver() : x_vScanner(*this, true), x_vParser(*this, x_vScanner) {}

int Driver::Parse() {
    return x_vParser.parse();
}

void Driver::PrintSymbols() {
    x_vScanner.SetPrint(true);
    while (x_vScanner.NextSymbol().token() != TokenType::kEnd);
    x_vScanner.SetPrint(false);
}

}
