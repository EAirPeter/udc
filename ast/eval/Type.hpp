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
    const static Type tyNull;
    const static Type tyVoid;
    const static Type tyInt;
    const static Type tyBool;
    const static Type tyString;

public:
    inline Type(const INonArrayType &tyElem = VoidType::vInstance, std::uint32_t cDimension = 0) noexcept :
        x_tyElem(tyElem), x_cDimension(cDimension)
    {}
    
    inline const INonArrayType &GetElemType() const noexcept {
        return x_tyElem;
    }

    constexpr std::uint32_t GetDimension() const noexcept {
        return x_cDimension;
    }

    constexpr bool IsArray() const noexcept {
        return x_cDimension;
    }

    constexpr bool Accepts(const Type &vt) const noexcept {
        return IsArray() ?
            vt.GetElemType() == NullType::vInstance || *this == vt :
            !vt.IsArray() && GetElemType().Accepts(vt.GetElemType());
    }

    constexpr bool operator ==(const Type &ty) const noexcept {
        return GetElemType() == ty.GetElemType() && GetDimension() == ty.GetDimension();
    }
    
    constexpr bool operator !=(const Type &ty) const noexcept {
        return !(*this == ty);
    }

    constexpr llvm::Type *GetLlvmType() const noexcept {
        return x_pLlvmType;
    }

    void MakeLlvmType(cg::CodeGenManager &cgm) noexcept;

private:
    std::reference_wrapper<const INonArrayType> x_tyElem;
    std::uint32_t x_cDimension;
    llvm::Type *x_pLlvmType;
};

inline std::ostream &operator <<(std::ostream &os, const Type &ty) {
    ty.GetElemType().Print(os);
    for (auto i = ty.GetDimension(); i; --i)
        os << "[]";
    return os;
}

}

#endif
