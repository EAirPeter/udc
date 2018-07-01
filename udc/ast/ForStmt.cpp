#include "../IndentHelper.hpp"
#include "ExprBase.hpp"
#include "ForStmt.hpp"

namespace udc::ast {

ForStmt::ForStmt(
    const location &loc,
    std::unique_ptr<ExprBase> &&upInit,
    std::unique_ptr<ExprBase> &&upCond,
    std::unique_ptr<ExprBase> &&upIncr,
    std::unique_ptr<NodeBase> &&upBody
) noexcept :
    NodeBase(loc),
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
