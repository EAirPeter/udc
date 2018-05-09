#ifndef UDC_AST_UNARY_EXPR_HPP_
#define UDC_AST_UNARY_EXPR_HPP_

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

enum class UnaOp {
    kNeg,
    kNot,
};

class UnaryExpr : public Base, public IExpression {
public:
    UnaryExpr(
        const Location &vLocation,
        UnaOp vOp,
        std::unique_ptr<IExpression> &&upExpr
    ) noexcept;
    virtual ~UnaryExpr();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

private:
    UnaOp x_vOp;
    std::unique_ptr<IExpression> x_upExpr;
};

}

#endif
