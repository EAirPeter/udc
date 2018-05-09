#ifndef UDC_AST_FOR_STMT_HPP_
#define UDC_AST_FOR_STMT_HPP_

#include <memory>

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class ForStmt : public Base, public IStatement {
public:
    ForStmt(
        const Location &vLocation,
        std::unique_ptr<IExpression> &&upInit,
        std::unique_ptr<IExpression> &&upCond,
        std::unique_ptr<IExpression> &&upIncr,
        std::unique_ptr<IStatement> &&upBody
    ) noexcept;
    virtual ~ForStmt();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

private:
    std::unique_ptr<IExpression> x_upInit;
    std::unique_ptr<IExpression> x_upCond;
    std::unique_ptr<IExpression> x_upIncr;
    std::unique_ptr<IStatement> x_upBody;
};

}

#endif
