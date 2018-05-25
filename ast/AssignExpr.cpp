#include "AssignExpr.hpp"

namespace udc::ast {

AssignExpr::AssignExpr(
    const location &loc,
    std::unique_ptr<ExprBase> &&upLhs,
    std::unique_ptr<ExprBase> &&upRhs
) noexcept :
    ExprBase(loc), x_upLhs(std::move(upLhs)), x_upRhs(std::move(upRhs))
{}

AssignExpr::~AssignExpr() {}

void AssignExpr::Print(std::ostream &os, std::uint32_t cIndent) const {
    x_upLhs->Print(os, cIndent);
    os << " = " << *x_upRhs;
}

}
