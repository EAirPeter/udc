#include "../IndentHelper.hpp"
#include "IntLit.hpp"

namespace udc::ast {

IntLit::IntLit(Driver &drv, const Location &loc, int nVal) noexcept : ExprBase(drv, loc), x_nVal(nVal) {}

IntLit::~IntLit() {}

void IntLit::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << x_nVal;
}

}
