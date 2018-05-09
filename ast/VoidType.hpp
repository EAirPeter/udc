#ifndef UDC_AST_VOID_TYPE_HPP_
#define UDC_AST_VOID_TYPE_HPP_

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class VoidType : public Base, public IType {
public:
    VoidType(const Location &vLocation) noexcept;
    virtual ~VoidType();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;
    virtual std::string GetName() const noexcept override;
};

}

#endif
