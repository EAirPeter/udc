#ifndef UDC_AST_VAR_DEF_HPP_
#define UDC_AST_VAR_DEF_HPP_

#include <memory>

#include "NodeBase.hpp"
#include "eval/Type.hpp"

namespace llvm {
class Value;
}

namespace udc::ast {

class VarDef : public NodeBase {
public:
    VarDef(const location &loc, std::unique_ptr<TypeName> &&upType, std::string &&sName) noexcept;
    virtual ~VarDef();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }

    inline TypeName *GetTypeName() const noexcept {
        return x_upType.get();
    }

    constexpr const std::string &GetName() const noexcept {
        return x_sName;
    }

    constexpr const eval::Type &GetType() const noexcept {
        return *x_pty;
    }

    constexpr void SetType(const eval::Type &ty) noexcept {
        x_pty = &ty;
    }

    constexpr ClassDef &GetClass() const noexcept {
        return *x_pClass;
    }

    constexpr void SetClass(ClassDef &vClass) noexcept {
        x_pClass = &vClass;
    }

    constexpr std::size_t GetIdx() const noexcept {
        return x_idx;
    }

    constexpr void SetIdx(std::size_t idx) noexcept {
        x_idx = idx;
    }

    constexpr bool IsField() const noexcept {
        return ~x_idx;
    }

    constexpr llvm::Value *GetLvPtr() const noexcept {
        return x_plvPtr;
    }

    constexpr void SetLvPtr(llvm::Value *plvPtr) noexcept {
        x_plvPtr = plvPtr;
    }

private:
    std::unique_ptr<TypeName> x_upType;
    std::string x_sName;
    const eval::Type *x_pty;
    ClassDef *x_pClass = nullptr;
    std::size_t x_idx = ~std::size_t {};
    llvm::Value *x_plvPtr;
};

}

#endif
