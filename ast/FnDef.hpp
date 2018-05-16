#ifndef UDC_AST_FN_DEF_HPP_
#define UDC_AST_FN_DEF_HPP_

#include "NodeBase.hpp"
#include "eval/Type.hpp"
#include "eval/SymbolTable.hpp"

namespace udc::ast {

class FnDef : public NodeBase {
public:
    FnDef(
        const Location &vLocation,
        bool bStatic,
        std::unique_ptr<TypeName> &&upType,
        std::string &&sName,
        std::vector<std::unique_ptr<VarDef>> &&vecPars,
        std::unique_ptr<BlockStmt> &&upBody
    ) noexcept;
    virtual ~FnDef();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }

    constexpr bool IsStatic() const noexcept {
        return x_bStatic;
    }

    inline TypeName *GetTypeName() const noexcept {
        return x_upType.get();
    }

    constexpr const std::string &GetName() const noexcept {
        return x_sName;
    }

    constexpr const std::vector<std::unique_ptr<VarDef>> &GetPars() const noexcept {
        return x_vecPars;
    }

    inline BlockStmt *GetBody() const noexcept {
        return x_upBody.get();
    }
    
    constexpr const eval::Type &GetType() const noexcept {
        return x_vType;
    }

    constexpr void SetType(const eval::Type &vType) noexcept {
        x_vType = vType;
    }

    constexpr eval::VarTable &GetVarTable() noexcept {
        return x_stVar;
    }

    constexpr std::size_t GetIdx() const noexcept {
        return x_idx;
    }

    constexpr void SetIdx(std::size_t idx) noexcept {
        x_idx = idx;
    }

private:
    bool x_bStatic;
    std::unique_ptr<TypeName> x_upType;
    std::string x_sName;
    std::vector<std::unique_ptr<VarDef>> x_vecPars;
    std::unique_ptr<BlockStmt> x_upBody;
    eval::Type x_vType;
    eval::VarTable x_stVar;
    std::size_t x_idx;
};

}

#endif
