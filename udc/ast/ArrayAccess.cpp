#include "../IndentHelper.hpp"
#include "ArrayAccess.hpp"

namespace udc::ast {

ArrayAccess::ArrayAccess(
    const location &loc,
    std::unique_ptr<ExprBase> &&upExpr,
    std::unique_ptr<ExprBase> &&upSub
) noexcept :
    ExprBase(loc, true), x_upExpr(std::move(upExpr)), x_upSub(std::move(upSub))
{}

ArrayAccess::~ArrayAccess() {}

void ArrayAccess::Print(std::ostream &os, std::uint32_t cIndent) const {
    x_upExpr->Print(os, cIndent);
    os << '[' << *x_upSub << ']';
}

}
