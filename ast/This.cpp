#include "../IndentHelper.hpp"
#include "This.hpp"

namespace udc::ast {

This::This(const location &loc) noexcept : ExprBase(loc) {}

This::~This() {}

void This::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "this";
}

}
