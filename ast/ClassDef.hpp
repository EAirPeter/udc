#ifndef UDC_AST_CLASS_DEF_HPP_
#define UDC_AST_CLASS_DEF_HPP_

#include <memory>
#include <optional>
#include <vector>

#include "NodeBase.hpp"
#include "eval/SymbolTable.hpp"
#include "eval/Type.hpp"

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

    constexpr void SetBase(const ClassDef &tyBase) noexcept {
        x_pBase = &tyBase;
    }

    constexpr eval::FnTable &GetFnTable() noexcept {
        return x_stFn;
    }

    constexpr eval::VarTable &GetVarTable() noexcept {
        return x_stVar;
    }

private:
    std::string x_sName;
    std::optional<std::string> x_soBase;
    std::vector<std::unique_ptr<NodeBase>> x_vecFields;
    const ClassDef *x_pBase;
    eval::FnTable x_stFn;
    eval::VarTable x_stVar;
};

}

#endif
