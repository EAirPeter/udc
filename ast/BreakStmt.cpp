#include "../IndentHelper.hpp"
#include "BreakStmt.hpp"

namespace udc::ast {

BreakStmt::BreakStmt(Driver &drv, const Location &loc) noexcept : NodeBase(drv, loc) {}

BreakStmt::~BreakStmt() {}

void BreakStmt::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "break";
}

}
