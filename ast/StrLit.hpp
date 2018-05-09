#ifndef UDC_AST_STR_LIT_HPP_
#define UDC_AST_STR_LIT_HPP_

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class StrLit : public Base, public ILiteral {
public:
    StrLit(const Location &vLocation, std::string &&sVal) noexcept;
    virtual ~StrLit();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

private:
    std::string x_sVal;
};

}

#endif
