#include "../IndentHelper.hpp"
#include "NewClassExpr.hpp"

namespace udc::ast {

NewClassExpr::NewClassExpr(const location &loc, std::string &&sName) noexcept :
    ExprBase(loc), x_sName(std::move(sName))
{}

NewClassExpr::~NewClassExpr() {}

void NewClassExpr::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "new " << x_sName << "()";
}

}
