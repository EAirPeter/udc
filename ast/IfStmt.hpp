#ifndef UDC_AST_IF_STMT_HPP_
#define UDC_AST_IF_STMT_HPP_

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class IfStmt : public Base, public IStatement {
public:
    IfStmt(
        const Location &vLocation,
        std::unique_ptr<IExpression> &&upCond,
        std::unique_ptr<IStatement> &&upThen,
        std::unique_ptr<IStatement> &&upElse
    ) noexcept;
    virtual ~IfStmt();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

private:
    std::unique_ptr<IExpression> x_upCond;
    std::unique_ptr<IStatement> x_upThen;
    std::unique_ptr<IStatement> x_upElse;
};

}

#endif
