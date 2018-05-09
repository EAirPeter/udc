#ifndef UDC_AST_RETURN_STMT_HPP_
#define UDC_AST_RETURN_STMT_HPP_

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class ReturnStmt : public Base, public IStatement {
public:
    ReturnStmt(const Location &vLocation, std::unique_ptr<IExpression> &&upExpr) noexcept;
    virtual ~ReturnStmt();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

private:
    std::unique_ptr<IExpression> x_upExpr;
};

}

#endif
