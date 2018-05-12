#include "../IndentHelper.hpp"
#include "NewArrayExpr.hpp"
#include "TypeName.hpp"

namespace udc::ast {

NewArrayExpr::NewArrayExpr(
    const Location &vLocation,
    std::unique_ptr<TypeName> &&upType,
    std::unique_ptr<ExprBase> &&upExpr
) noexcept : ExprBase(vLocation), x_upType(std::move(upType)), x_upExpr(std::move(upExpr))
{}

NewArrayExpr::~NewArrayExpr() {}

void NewArrayExpr::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "new " << *x_upType << '[' << *x_upExpr << ']';
}

}
