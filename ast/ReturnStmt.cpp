#include "../IndentHelper.hpp"
#include "ExprBase.hpp"
#include "ReturnStmt.hpp"

namespace udc::ast {

ReturnStmt::ReturnStmt(Driver &drv, const Location &loc, std::unique_ptr<ExprBase> &&upExpr) noexcept :
    NodeBase(drv, loc), x_upExpr(std::move(upExpr))
{}

ReturnStmt::~ReturnStmt() {}

void ReturnStmt::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "return";
    if (x_upExpr)
        os << ' ' << *x_upExpr;
}

}
