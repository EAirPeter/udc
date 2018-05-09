#ifndef UDC_AST_CAST_EXPR_HPP_
#define UDC_AST_CAST_EXPR_HPP_

#include <memory>

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class CastExpr : public Base, public IExpression {
public:
    CastExpr(
        const Location &vLocation,
        std::string &&sName,
        std::unique_ptr<IExpression> &&upExpr
    ) noexcept;
    virtual ~CastExpr();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

private:
    std::string x_sName;
    std::unique_ptr<IExpression> x_upExpr;
};

}

#endif
