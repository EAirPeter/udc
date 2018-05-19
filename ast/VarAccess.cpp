#include "../IndentHelper.hpp"
#include "VarAccess.hpp"

namespace udc::ast {

VarAccess::VarAccess(
    Driver &drv,
    const Location &loc,
    std::unique_ptr<ExprBase> &&upExpr,
    std::string &&sName
) noexcept :
    ExprBase(drv, loc, true), x_upExpr(std::move(upExpr)), x_sName(std::move(sName))
{}

VarAccess::~VarAccess() {}

void VarAccess::Print(std::ostream &os, std::uint32_t cIndent) const {
    if (x_upExpr) {
        x_upExpr->Print(os, cIndent);
        os << '.';
    }
    else {
        os << Indent(cIndent);
    }
    os << x_sName;
}

}
