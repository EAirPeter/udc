#include "../IndentHelper.hpp"
#include "UnaryExpr.hpp"

namespace udc::ast {

UnaryExpr::UnaryExpr(
    const location &loc,
    UnaOp vOp,
    std::unique_ptr<ExprBase> &&upExpr
) noexcept :
    ExprBase(loc), x_vOp(vOp), x_upExpr(std::move(upExpr))
{}

UnaryExpr::~UnaryExpr() {}

namespace {

constexpr const char *UnaOpToNtmbs(UnaOp vOp) noexcept {
    constexpr const char *aUnaOps[] {"-", "!"};
    return aUnaOps[static_cast<int>(vOp)];
}

}

void UnaryExpr::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << UnaOpToNtmbs(x_vOp) << *x_upExpr;
}

}
