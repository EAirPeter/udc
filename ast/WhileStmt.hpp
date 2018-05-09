#ifndef UDC_AST_WHILE_STMT_HPP_
#define UDC_AST_WHILE_STMT_HPP_

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class WhileStmt : public Base, public IStatement {
public:
    WhileStmt(
        const Location &vLocation,
        std::unique_ptr<IExpression> &&upCond,
        std::unique_ptr<IStatement> &&upBody
    ) noexcept;
    virtual ~WhileStmt();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

private:
    std::unique_ptr<IExpression> x_upCond;
    std::unique_ptr<IStatement> x_upBody;
};

}

#endif
