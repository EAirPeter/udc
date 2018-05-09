#ifndef UDC_AST_STR_TYPE_HPP_
#define UDC_AST_STR_TYPE_HPP_

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class StrType : public Base, public IType {
public:
    StrType(const Location &vLocation) noexcept;
    virtual ~StrType();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;
    virtual std::string GetName() const noexcept override;
};

}

#endif
