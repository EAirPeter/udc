#ifndef UDC_AST_ASSIGN_EXPR_HPP_
#define UDC_AST_ASSIGN_EXPR_HPP_

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class AssignExpr : public Base, public IExpression {
public:
    AssignExpr(
        const Location &vLocation,
        std::unique_ptr<ILValue> &&upLhs,
        std::unique_ptr<IExpression> &&upRhs
    ) noexcept;
    virtual ~AssignExpr();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

private:
    std::unique_ptr<ILValue> x_upLhs;
    std::unique_ptr<IExpression> x_upRhs;
};

}

#endif
