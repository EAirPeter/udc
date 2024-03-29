#include "../IndentHelper.hpp"
#include "BoolLit.hpp"

namespace udc::ast {

BoolLit::BoolLit(const location &loc, bool bVal) noexcept : ExprBase(loc), x_bVal(bVal) {}

BoolLit::~BoolLit() {}

void BoolLit::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << (x_bVal ? "true" : "false");
}

}
