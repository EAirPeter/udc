#ifndef UDC_AST_CLASS_DEF_HPP_
#define UDC_AST_CLASS_DEF_HPP_

#include <optional>
#include <vector>

#include "NodeBase.hpp"
#include "eval/SymbolTable.hpp"
#include "eval/Type.hpp"
#include "eval/VirtFnTable.hpp"

namespace udc::ast {

class ClassDef : public NodeBase, public eval::INonArrayType {
public:
    ClassDef(
        const Location &vLocation,
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

    constexpr eval::VarTable &GetVarTable() noexcept {
        return x_stVar;
    }

    constexpr const eval::VarTable &GetVarTable() const noexcept {
        return x_stVar;
    }

private:
    ClassDef *X_GetRoot() noexcept;

private:
    std::string x_sName;
    std::optional<std::string> x_soBase;
    std::vector<std::unique_ptr<NodeBase>> x_vecFields;
    ClassDef *x_pRoot = nullptr;
    const ClassDef *x_pBase;
    std::vector<ClassDef *> x_vecDeriveds;
    eval::FnTable x_stFn;
    eval::VarTable x_stVar;
    eval::VfTable x_stVf;
};

}

#endif
