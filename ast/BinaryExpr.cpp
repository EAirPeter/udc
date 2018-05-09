#include "BinaryExpr.hpp"

namespace udc::ast {

BinaryExpr::BinaryExpr(
    const Location &vLocation,
    BinOp vOp,
    std::unique_ptr<IExpression> &&upLhs,
    std::unique_ptr<IExpression> &&upRhs
) noexcept :
    Base(vLocation), x_vOp(vOp), x_upLhs(std::move(upLhs)), x_upRhs(std::move(upRhs))
{}

BinaryExpr::~BinaryExpr() {}

namespace {

constexpr const char *BinOpToNtmbs(BinOp vOp) noexcept {
    constexpr const char *aBinOps[] {
        "||", "&&", "==", "!=", "<", ">", "<=", ">=",
        "+", "-", "*", "/", "%",
    };
    return aBinOps[static_cast<int>(vOp)];
}

}

void BinaryExpr::Print(std::ostream &os, std::uint32_t cIndent) const {
    x_upLhs->Print(os, cIndent);
    os << ' ' << BinOpToNtmbs(x_vOp) << ' ' << *x_upRhs;
}

}
