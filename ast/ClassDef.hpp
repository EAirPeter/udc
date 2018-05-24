#ifndef UDC_AST_CLASS_DEF_HPP_
#define UDC_AST_CLASS_DEF_HPP_

#include <optional>
#include <vector>

#include "NodeBase.hpp"
#include "eval/SymbolTable.hpp"
#include "eval/Type.hpp"
#include "eval/VirtFnTable.hpp"

namespace llvm {
class StructType;
class GlobalVariable;
}

namespace udc::ast {

class ClassDef : public NodeBase, public eval::INonArrayType {
public:
    ClassDef(
        Driver &drv,
        const Location &loc,
        std::string &&sName,
        std::optional<std::string> &&soBase,
        std::vector<std::unique_ptr<NodeBase>> &&vecFields
    ) noexcept;
    virtual ~ClassDef();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;
    virtual bool Accepts(const INonArrayType &ty) const noexcept override;
    virtual void Print(std::ostream &os) const override;
    
    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }
    
    constexpr const std::string &GetName() const noexcept {
        return x_sName;
    }

    constexpr const std::optional<std::string> &GetBaseName() const noexcept {
        return x_soBase;
    }

    constexpr const std::vector<std::unique_ptr<NodeBase>> &GetFields() const noexcept {
        return x_vecFields;
    }

    constexpr const eval::Type &GetType() const noexcept {
        return *x_pty;
    }

    constexpr void SetType(const eval::Type &ty) noexcept {
        x_pty = &ty;
    }

    constexpr const ClassDef *GetBase() const noexcept {
        return x_pBase;
    }

    bool SetBase(ClassDef &tyBase) noexcept;

    constexpr const std::vector<ClassDef *> &GetDeriveds() const noexcept {
        return x_vecDeriveds;
    }

    constexpr eval::VfTable &GetVfTable() noexcept {
        return x_stVf;
    }

    constexpr const eval::VfTable &GetVfTable() const noexcept {
        return x_stVf;
    }

    constexpr eval::FnTable &GetFnTable() noexcept {
        return x_stFn;
    }

    constexpr const eval::FnTable &GetFnTable() const noexcept {
        return x_stFn;
    }

    constexpr std::vector<VarDef *> &GetVars() noexcept {
        return x_vecVars;
    }

    constexpr const std::vector<VarDef *> &GetVars() const noexcept {
        return x_vecVars;
    }
    
    constexpr eval::VarTable &GetVarTable() noexcept {
        return x_stVar;
    }

    constexpr const eval::VarTable &GetVarTable() const noexcept {
        return x_stVar;
    }

    constexpr std::size_t GetIdx() const noexcept {
        return x_idx;
    }
    
    constexpr void SetIdx(std::size_t idx) noexcept {
        x_idx = idx;
    }

    constexpr llvm::StructType *GetLvType() const noexcept {
        return x_plvType;
    }

    constexpr void SetLvType(llvm::StructType *plvType) noexcept {
        x_plvType = plvType;
    }

    constexpr llvm::GlobalVariable *GetLvVTable() const noexcept {
        return x_plvVTable;
    }

    constexpr void SetLvVTable(llvm::GlobalVariable *plvVTable) noexcept {
        x_plvVTable = plvVTable;
    }

private:
    ClassDef *X_GetRoot() noexcept;

private:
    std::string x_sName;
    std::optional<std::string> x_soBase;
    std::vector<std::unique_ptr<NodeBase>> x_vecFields;
    const eval::Type *x_pty;
    ClassDef *x_pRoot = nullptr;
    const ClassDef *x_pBase;
    std::vector<ClassDef *> x_vecDeriveds;
    eval::FnTable x_stFn;
    std::vector<VarDef *> x_vecVars;
    eval::VarTable x_stVar;
    eval::VfTable x_stVf;
    std::size_t x_idx;
    llvm::StructType *x_plvType;
    llvm::GlobalVariable *x_plvVTable;
};

}

#endif
