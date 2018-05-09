#ifndef UDC_AST_THIS_HPP_
#define UDC_AST_THIS_HPP_

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class This : public Base, public IExpression {
public:
    This(const Location &vLocation) noexcept;
    virtual ~This();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;
};

}

#endif
