#ifndef UDC_AST_NULL_LIT_HPP_
#define UDC_AST_NULL_LIT_HPP_

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class NullLit : public Base, public ILiteral {
public:
    NullLit(const Location &vLocation) noexcept;
    virtual ~NullLit();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;
};

}

#endif
