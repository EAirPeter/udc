#include "../IndentHelper.hpp"
#include "BreakStmt.hpp"

namespace udc::ast {

BreakStmt::BreakStmt(const location &loc) noexcept : NodeBase(loc) {}

BreakStmt::~BreakStmt() {}

void BreakStmt::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "break";
}

}
