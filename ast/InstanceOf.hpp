#ifndef UDC_AST_INSTANCE_OF_HPP_
#define UDC_AST_INSTANCE_OF_HPP_

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class InstanceOf : public Base, public IExpression {
public:
    InstanceOf(const Location &vLocation, std::unique_ptr<IExpression> &&upExpr, std::string &&sName) noexcept;
    virtual ~InstanceOf();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

private:
    std::unique_ptr<IExpression> x_upExpr;
    std::string x_sName;
};

}

#endif
