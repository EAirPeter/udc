#include "../IndentHelper.hpp"
#include "ReadInteger.hpp"

namespace udc::ast {

ReadInteger::ReadInteger(const Location &vLocation) noexcept : ExprBase(vLocation) {}

ReadInteger::~ReadInteger() {}

void ReadInteger::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "ReadInteger()";
}

}
