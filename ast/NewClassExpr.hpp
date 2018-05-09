#ifndef UDC_AST_NEW_CLASS_EXPR_HPP_
#define UDC_AST_NEW_CLASS_EXPR_HPP_

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class NewClassExpr : public Base, public IExpression {
public:
    NewClassExpr(
        const Location &vLocation,
        std::string &&sName
    ) noexcept;
    virtual ~NewClassExpr();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

private:
    std::string x_sName;
};

}

#endif
