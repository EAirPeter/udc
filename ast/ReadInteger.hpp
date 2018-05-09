#ifndef UDC_AST_READ_INTEGER_HPP_
#define UDC_AST_READ_INTEGER_HPP_

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class ReadInteger : public Base, public IExpression {
public:
    ReadInteger(const Location &vLocation) noexcept;
    virtual ~ReadInteger();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;
};

}

#endif
