#ifndef UDC_AST_VAR_ACCESS_HPP_
#define UDC_AST_VAR_ACCESS_HPP_

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class VarAccess : public Base, public ILValue {
public:
    VarAccess(
        const Location &vLocation,
        std::unique_ptr<IExpression> &&upExpr,
        std::string &&sName
    ) noexcept;
    virtual ~VarAccess();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

private:
    std::unique_ptr<IExpression> x_upExpr;
    std::string x_sName;
};

}

#endif
