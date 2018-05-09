#ifndef UDC_AST_ARRAY_TYPE_HPP_
#define UDC_AST_ARRAY_TYPE_HPP_

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class ArrayType : public Base, public IType {
public:
    ArrayType(const Location &vLocation, std::unique_ptr<IType> &&upType) noexcept;
    virtual ~ArrayType();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;
    virtual std::string GetName() const noexcept override;

public:
    IType *GetElementType() const noexcept;

private:
    std::unique_ptr<IType> x_upType;
};

}

#endif
