#include "../IndentHelper.hpp"
#include "ReturnStmt.hpp"

namespace udc::ast {

ReturnStmt::ReturnStmt(const Location &vLocation, std::unique_ptr<IExpression> &&upExpr) noexcept :
    Base(vLocation), x_upExpr(std::move(upExpr))
{}

ReturnStmt::~ReturnStmt() {}

void ReturnStmt::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "return";
    if (x_upExpr)
        os << ' ' << *x_upExpr;
}

}
