#include "../IndentHelper.hpp"
#include "ReadLine.hpp"

namespace udc::ast {

ReadLine::ReadLine(const Location &vLocation) noexcept : ExprBase(vLocation) {}

ReadLine::~ReadLine() {}

void ReadLine::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "ReadLine()";
}

}
