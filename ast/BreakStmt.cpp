#include "../IndentHelper.hpp"
#include "BreakStmt.hpp"

namespace udc::ast {

BreakStmt::BreakStmt(const Location &vLocation) noexcept : Base(vLocation) {}

BreakStmt::~BreakStmt() {}

void BreakStmt::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "break";
}

}
