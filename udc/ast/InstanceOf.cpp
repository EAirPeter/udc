#include "../IndentHelper.hpp"
#include "InstanceOf.hpp"

namespace udc::ast {

InstanceOf::InstanceOf(
    const location &loc,
    std::unique_ptr<ExprBase> &&upExpr,
    std::string &&sName
) noexcept :
    ExprBase(loc), x_upExpr(std::move(upExpr)), x_sName(std::move(sName))
{}

InstanceOf::~InstanceOf() {}

void InstanceOf::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "instanceof(" << *x_upExpr << ", " << x_sName << ')';
}

}
