#include "../IndentHelper.hpp"
#include "NullLit.hpp"

namespace udc::ast {

NullLit::NullLit(Driver &drv, const Location &loc) noexcept : ExprBase(drv, loc) {}

NullLit::~NullLit() {}

void NullLit::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "null";
}

}
