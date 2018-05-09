#ifndef UDC_AST_NEW_BOOL_LIT_HPP_
#define UDC_AST_NEW_BOOL_LIT_HPP_

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class BoolLit : public Base, public ILiteral {
public:
    BoolLit(const Location &vLocation, bool bVal) noexcept;
    virtual ~BoolLit();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

private:
    bool x_bVal;
};

}

#endif
