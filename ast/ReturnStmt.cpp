#include "../IndentHelper.hpp"
#include "ExprBase.hpp"
#include "ReturnStmt.hpp"

namespace udc::ast {

ReturnStmt::ReturnStmt(const location &loc, std::unique_ptr<ExprBase> &&upExpr) noexcept :
    NodeBase(loc), x_upExpr(std::move(upExpr))
{}

ReturnStmt::~ReturnStmt() {}

void ReturnStmt::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "return";
    if (x_upExpr)
        os << ' ' << *x_upExpr;
}

}
