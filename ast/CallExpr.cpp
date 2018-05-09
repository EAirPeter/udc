#include "../IndentHelper.hpp"
#include "CallExpr.hpp"

namespace udc::ast {

CallExpr::CallExpr(
    const Location &vLocation,
    std::unique_ptr<IExpression> &&upExpr,
    std::string &&sName,
    std::vector<std::unique_ptr<IExpression>> &&vecArgs
) noexcept :
    Base(vLocation),
    x_upExpr(std::move(upExpr)),
    x_sName(std::move(sName)),
    x_vecArgs(std::move(vecArgs))
{}

CallExpr::~CallExpr() {}

void CallExpr::Print(std::ostream &os, std::uint32_t cIndent) const {
    if (x_upExpr) {
        x_upExpr->Print(os, cIndent);
        os << '.';
    }
    else
        os << Indent(cIndent);
    os << x_sName << '(';
    bool bFirst = true;
    for (auto &&upArg : x_vecArgs) {
        if (bFirst)
            bFirst = false;
        else
            os << ", ";
        os << *upArg;
    }
    os << ')';
}

}