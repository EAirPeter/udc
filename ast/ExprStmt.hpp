#ifndef UDC_AST_EXPR_STMT_HPP_
#define UDC_AST_EXPR_STMT_HPP_

#include <memory>

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class ExprStmt : public Base, public virtual IStatement {
public:
    ExprStmt(const Location &vLocation, std::unique_ptr<IExpression> &&upExpr) noexcept;
    virtual ~ExprStmt();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

private:
    std::unique_ptr<IExpression> x_upExpr;
};

}

#endif
