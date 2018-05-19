#include "../IndentHelper.hpp"
#include "ExprBase.hpp"
#include "ExprStmt.hpp"

namespace udc::ast {

ExprStmt::ExprStmt(Driver &drv, const Location &loc, std::unique_ptr<ExprBase> &&upExpr) noexcept :
    NodeBase(drv, loc), x_upExpr(std::move(upExpr))
{}

ExprStmt::~ExprStmt() {}

void ExprStmt::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent);
    if (x_upExpr)
        os << *x_upExpr;
    else
        os << "<empty>";
}

}
