#ifndef UDC_AST_BINARY_EXPR_HPP_
#define UDC_AST_BINARY_EXPR_HPP_

#include <memory>

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

enum class BinOp {
    kIor,
    kAnd,
    kEqu,
    kNeq,
    kLes,
    kGre,
    kLeq,
    kGeq,
    kAdd,
    kSub,
    kMul,
    kDiv,
    kMod,
};

class BinaryExpr : public Base, public IExpression {
public:
    BinaryExpr(
        const Location &vLocation,
        BinOp vOp,
        std::unique_ptr<IExpression> &&upLhs,
        std::unique_ptr<IExpression> &&upRhs
    ) noexcept;
    virtual ~BinaryExpr();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

private:
    BinOp x_vOp;
    std::unique_ptr<IExpression> x_upLhs;
    std::unique_ptr<IExpression> x_upRhs;
};

}

#endif
