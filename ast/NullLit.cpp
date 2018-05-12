#include "../IndentHelper.hpp"
#include "NullLit.hpp"

namespace udc::ast {

NullLit::NullLit(const Location &vLocation) noexcept : ExprBase(vLocation) {}

NullLit::~NullLit() {}

void NullLit::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "null";
}

}
