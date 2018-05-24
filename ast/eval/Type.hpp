#ifndef UDC_AST_EVAL_TYPE_HPP_
#define UDC_AST_EVAL_TYPE_HPP_

#include "../NodeBase.hpp"
#include "../TypeName.hpp"
#include "Fwd.hpp"

namespace llvm {
class Type;
}

namespace udc::cg {
class CodeGenManager;
}

namespace udc::ast::eval {

struct INonArrayType {
    virtual inline ~INonArrayType() = default;
    virtual bool Accepts(const INonArrayType &ty) const noexcept = 0;
    virtual void Print(std::ostream &os) const = 0;
};

constexpr bool operator ==(const INonArrayType &lhs, const INonArrayType &rhs) noexcept {
    return std::addressof(lhs) == std::addressof(rhs);
}

namespace _impl_BuiltinType {
template<BuiltinTypeId vId>
constexpr const char *BuiltinTypeIdNtmbs() noexcept {
    constexpr const char *aNames[] {
        "null", "void", "int", "bool", "string"
    };
    return aNames[(int) vId];
}

template<BuiltinTypeId vId>
class BuiltinType : public INonArrayType {
public:
    static const BuiltinType vInstance;
public:
    virtual inline ~BuiltinType() = default;

    virtual inline bool Accepts(const INonArrayType &ty) const noexcept override {
        if constexpr (vId == BuiltinTypeId::kVoid)
            return false;
        else
            return dynamic_cast<const BuiltinType *>(&ty) == this;
    }

    virtual inline void Print(std::ostream &os) const override {
        os << BuiltinTypeIdNtmbs<vId>();
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
    inline Type(const INonArrayType &vElem, std::uint32_t cDimension) noexcept :
        x_vElem(vElem), x_cDimension(cDimension)
    {}
    
    inline const INonArrayType &GetElemType() const noexcept {
        return x_vElem;
    }

    constexpr std::uint32_t GetDimension() const noexcept {
        return x_cDimension;
    }

    constexpr bool IsArray() const noexcept {
        return x_cDimension;
    }

    constexpr bool Accepts(const Type &ty) const noexcept {
        return IsArray() ?
            ty.GetElemType() == NullType::vInstance || *this == ty :
            !ty.IsArray() && GetElemType().Accepts(ty.GetElemType());
    }

    constexpr bool operator ==(const Type &ty) const noexcept {
        return this == &ty;
    }
    
    constexpr bool operator !=(const Type &ty) const noexcept {
        return this != &ty;
    }

    constexpr llvm::Type *GetLvType() const noexcept {
        return x_plvType;
    }

    constexpr void SetLvType(llvm::Type *plvType) const noexcept {
        x_plvType = plvType;
    }

private:
    const INonArrayType &x_vElem;
    std::uint32_t x_cDimension;
    mutable llvm::Type *x_plvType = nullptr;
};

inline std::ostream &operator <<(std::ostream &os, const Type &ty) {
    ty.GetElemType().Print(os);
    for (auto i = ty.GetDimension(); i; --i)
        os << "[]";
    return os;
}

}

#endif
