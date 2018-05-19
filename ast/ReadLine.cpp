#include "../IndentHelper.hpp"
#include "ReadLine.hpp"

namespace udc::ast {

ReadLine::ReadLine(Driver &drv, const Location &loc) noexcept : ExprBase(drv, loc) {}

ReadLine::~ReadLine() {}

void ReadLine::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "ReadLine()";
}

}
