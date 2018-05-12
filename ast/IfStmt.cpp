#include "../IndentHelper.hpp"
#include "ExprBase.hpp"
#include "IfStmt.hpp"

namespace udc::ast {

IfStmt::IfStmt(
    const Location &vLocation,
    std::unique_ptr<ExprBase> &&upCond,
    std::unique_ptr<NodeBase> &&upThen,
    std::unique_ptr<NodeBase> &&upElse
) noexcept :
    NodeBase(vLocation),
    x_upCond(std::move(upCond)),
    x_upThen(std::move(upThen)),
    x_upElse(std::move(upElse))
{}

IfStmt::~IfStmt() {}

void udc::ast::IfStmt::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "if (" << *x_upCond << ")" << std::endl;
    x_upThen->Print(os, cIndent + 1);
    if (x_upElse) {
        os << std::endl << Indent(cIndent) << "else" << std::endl;
        x_upElse->Print(os, cIndent + 1);
    }
}

}
