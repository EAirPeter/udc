#ifndef UDC_AST_NEW_ARRAY_EXPR_HPP_
#define UDC_AST_NEW_ARRAY_EXPR_HPP_

#include <memory>

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class NewArrayExpr : public Base, public IExpression {
public:
    NewArrayExpr(
        const Location &vLocation,
        std::unique_ptr<IType> &&upType,
        std::unique_ptr<IExpression> &&upExpr
    ) noexcept;
    virtual ~NewArrayExpr();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

private:
    std::unique_ptr<IType> x_upType;
    std::unique_ptr<IExpression> x_upExpr;
};

}

#endif
