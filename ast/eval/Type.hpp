#ifndef UDC_AST_EVAL_TYPE_HPP_
#define UDC_AST_EVAL_TYPE_HPP_

#include "Fwd.hpp"
#include "../NodeBase.hpp"
#include "../TypeName.hpp"

namespace udc::ast::eval {

struct INonArrayType {
    virtual inline ~INonArrayType() = default;
    virtual bool Accepts(const INonArrayType &ty) const noexcept = 0;
};

namespace _impl_BuiltinType {
template<BuiltinTypeId vId>
class BuiltinType : public INonArrayType {
public:
    static const BuiltinType vInstance;
public:
    virtual inline ~BuiltinType() = default;

    virtual inline bool Accepts(const INonArrayType &ty) const noexcept override {
        return dynamic_cast<const BuiltinType *>(&ty) == this;
    }

private:
    inline BuiltinType() noexcept = default;
};
}

using NullType = _impl_BuiltinType::BuiltinType<_impl_BuiltinType::BuiltinTypeId::kNull>;
using VoidType = _impl_BuiltinType::BuiltinType<_impl_BuiltinType::BuiltinTypeId::kVoid>;
using IntType = _impl_BuiltinType::BuiltinType<_impl_BuiltinType::BuiltinTypeId::kInt>;
using BoolType = _impl_BuiltinType::BuiltinType<_impl_BuiltinType::BuiltinTypeId::kBool>;
using StringType = _impl_BuiltinType::BuiltinType<_impl_BuiltinType::BuiltinTypeId::kString>;

class Type {
public:
    constexpr Type(const INonArrayType &tyElem = VoidType::vInstance, std::uint32_t cDimension = 0) noexcept :
        x_tyElem(&tyElem), x_cDimension(cDimension)
    {}
    
    constexpr const INonArrayType &GetElemType() const noexcept {
        return *x_tyElem;
    }

    constexpr std::uint32_t GetDimension() const noexcept {
        return x_cDimension;
    }

    constexpr bool IsArray() const noexcept {
        return x_cDimension;
    }

    inline bool Accepts(const Type &vt) const noexcept {
        return GetDimension() == vt.GetDimension() && GetElemType().Accepts(vt.GetElemType());
    }

private:
    const INonArrayType *x_tyElem;
    std::uint32_t x_cDimension;
};

}

#endif
