#include "../IndentHelper.hpp"
#include "ExprBase.hpp"
#include "ExprStmt.hpp"

namespace udc::ast {

ExprStmt::ExprStmt(const Location &vLocation, std::unique_ptr<ExprBase> &&upExpr) noexcept :
    NodeBase(vLocation), x_upExpr(std::move(upExpr))
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
