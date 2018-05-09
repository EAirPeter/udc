#ifndef UDC_AST_READ_LINE_HPP_
#define UDC_AST_READ_LINE_HPP_

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class ReadLine : public Base, public IExpression {
public:
    ReadLine(const Location &vLocation) noexcept;
    virtual ~ReadLine();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;
};

}

#endif
