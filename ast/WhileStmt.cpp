#include "../IndentHelper.hpp"
#include "ExprBase.hpp"
#include "WhileStmt.hpp"

namespace udc::ast {

WhileStmt::WhileStmt(
    const Location &vLocation,
    std::unique_ptr<ExprBase> &&upCond,
    std::unique_ptr<NodeBase> &&upBody
) noexcept :
    NodeBase(vLocation), x_upCond(std::move(upCond)), x_upBody(std::move(upBody))
{}

udc::ast::WhileStmt::~WhileStmt() {}

void udc::ast::WhileStmt::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "while (" << *x_upCond << ")" << std::endl;
    x_upBody->Print(os, cIndent + 1);
}

}
