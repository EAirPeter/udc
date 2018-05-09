#ifndef UDC_AST_BOOL_TYPE_HPP_
#define UDC_AST_BOOL_TYPE_HPP_

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class BoolType : public Base, public IType {
public:
    BoolType(const Location &vLocation) noexcept;
    virtual ~BoolType();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;
    virtual std::string GetName() const noexcept override;

};

}

#endif
