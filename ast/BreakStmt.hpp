#ifndef UDC_AST_BREAK_STMT_HPP_
#define UDC_AST_BREAK_STMT_HPP_

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class BreakStmt : public Base, public IStatement {
public:
    BreakStmt(const Location &vLocation) noexcept;
    virtual ~BreakStmt();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

private:
    std::unique_ptr<IExpression> x_upCond;
    std::unique_ptr<IStatement> x_upBody;
};

}

#endif
