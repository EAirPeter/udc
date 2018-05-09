#include "../IndentHelper.hpp"
#include "ForStmt.hpp"

namespace udc::ast {

ForStmt::ForStmt(
    const Location &vLocation,
    std::unique_ptr<IExpression> &&upInit,
    std::unique_ptr<IExpression> &&upCond,
    std::unique_ptr<IExpression> &&upIncr,
    std::unique_ptr<IStatement> &&upBody
) noexcept :
    Base(vLocation),
    x_upInit(std::move(upInit)),
    x_upCond(std::move(upCond)),
    x_upIncr(std::move(upIncr)),
    x_upBody(std::move(upBody))
{}

ForStmt::~ForStmt() {}

void udc::ast::ForStmt::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "for (";
    if (x_upInit)
        os << *x_upInit;
    os << ';';
    if (x_upCond)
        os << ' ' << *x_upCond;
    os << ';';
    if (x_upIncr)
        os << ' ' << *x_upIncr;
    os << ')' << std::endl;
    x_upBody->Print(os, cIndent + 1);
}

}
