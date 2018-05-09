#ifndef UDC_AST_INT_LIT_HPP_
#define UDC_AST_INT_LIT_HPP_

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class IntLit : public Base, public ILiteral {
public:
    IntLit(const Location &vLocation, int nVal) noexcept;
    virtual ~IntLit();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

private:
    int x_nVal;
};

}

#endif
