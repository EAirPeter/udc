#include "../IndentHelper.hpp"
#include "CastExpr.hpp"

namespace udc::ast {

CastExpr::CastExpr(
    Driver &drv,
    const Location &loc,
    std::string &&sName,
    std::unique_ptr<ExprBase> &&upExpr
) noexcept :
    ExprBase(drv, loc), x_sName(std::move(sName)), x_upExpr(std::move(upExpr))
{}

CastExpr::~CastExpr() {}

void CastExpr::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "(class " << x_sName << ") " << *x_upExpr;
}

}
