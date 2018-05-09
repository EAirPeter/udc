#include "../IndentHelper.hpp"
#include "IfStmt.hpp"

namespace udc::ast {

IfStmt::IfStmt(
    const Location &vLocation,
    std::unique_ptr<IExpression> &&upCond,
    std::unique_ptr<IStatement> &&upThen,
    std::unique_ptr<IStatement> &&upElse
) noexcept :
    Base(vLocation),
    x_upCond(std::move(upCond)),
    x_upThen(std::move(upThen)),
    x_upElse(std::move(upElse))
{}

IfStmt::~IfStmt() {}

void udc::ast::IfStmt::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "if (" << *x_upCond << ")" << std::endl;
    x_upThen->Print(os, cIndent + 1);
    if (x_upElse) {
        os << Indent(cIndent) << "else" << std::endl;
        x_upElse->Print(os, cIndent + 1);
    }
}

}
