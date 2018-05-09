#include "AssignExpr.hpp"

namespace udc::ast {

AssignExpr::AssignExpr(
    const Location &vLocation,
    std::unique_ptr<ILValue> &&upLhs,
    std::unique_ptr<IExpression> &&upRhs
) noexcept :
    Base(vLocation), x_upLhs(std::move(upLhs)), x_upRhs(std::move(upRhs))
{}

AssignExpr::~AssignExpr() {}

void AssignExpr::Print(std::ostream &os, std::uint32_t cIndent) const {
    x_upLhs->Print(os, cIndent);
    os << " = " << *x_upRhs;
}

}
