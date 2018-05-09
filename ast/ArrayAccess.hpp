#ifndef UDC_AST_ARRAY_ACCESS_HPP_
#define UDC_AST_ARRAY_ACCESS_HPP_

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class ArrayAccess : public Base, public ILValue {
public:
    ArrayAccess(
        const Location &vLocation,
        std::unique_ptr<IExpression> &&upExpr,
        std::unique_ptr<IExpression> &&upSub
    ) noexcept;
    virtual ~ArrayAccess();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

private:
    std::unique_ptr<IExpression> x_upExpr;
    std::unique_ptr<IExpression> x_upSub;
};

}

#endif
