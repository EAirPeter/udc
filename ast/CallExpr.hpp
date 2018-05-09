#ifndef UDC_AST_CALL_EXPR_HPP_
#define UDC_AST_CALL_EXPR_HPP_

#include <vector>

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class CallExpr : public Base, public IExpression {
public:
    CallExpr(
        const Location &vLocation,
        std::unique_ptr<IExpression> &&upExpr,
        std::string &&sName,
        std::vector<std::unique_ptr<IExpression>> &&vecArgs
    ) noexcept;
    virtual ~CallExpr();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

private:
    std::unique_ptr<IExpression> x_upExpr;
    std::string x_sName;
    std::vector<std::unique_ptr<IExpression>> x_vecArgs;
};

}

#endif
