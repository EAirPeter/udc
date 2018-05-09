#include "../IndentHelper.hpp"
#include "NewArrayExpr.hpp"

namespace udc::ast {

NewArrayExpr::NewArrayExpr(
    const Location &vLocation,
    std::unique_ptr<IType> &&upType,
    std::unique_ptr<IExpression> &&upExpr
) noexcept : Base(vLocation), x_upType(std::move(upType)), x_upExpr(std::move(upExpr))
{}

NewArrayExpr::~NewArrayExpr() {}

void NewArrayExpr::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "new " << x_upType->GetName() << '[' << *x_upExpr << ']';
}

}
