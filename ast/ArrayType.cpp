#include "ArrayType.hpp"

namespace udc::ast {

ArrayType::ArrayType(const Location &vLocation, std::unique_ptr<IType> &&upType) noexcept :
    Base(vLocation), x_upType(std::move(upType))
{}

ArrayType::~ArrayType() {}

void ArrayType::Print(std::ostream &os, std::uint32_t cIndent) const {
    x_upType->Print(os, cIndent);
    os << "[]";
}

std::string ArrayType::GetName() const noexcept {
    return x_upType->GetName() + "[]";
}

IType *ArrayType::GetElementType() const noexcept {
    return x_upType.get();
}

}
