#ifndef UDC_AST_VAR_ACCESS_HPP_
#define UDC_AST_VAR_ACCESS_HPP_

#include <memory>

#include "ExprBase.hpp"

namespace udc::ast {

class VarAccess final : public ExprBase {
public:
    VarAccess(
        const location &loc,
        std::unique_ptr<ExprBase> &&upExpr,
        std::string &&sName
    ) noexcept;
    virtual ~VarAccess();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;
    
    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }

    inline ExprBase *GetExpr() const noexcept {
        return x_upExpr.get();
    }

    constexpr const std::string &GetName() const noexcept {
        return x_sName;
    }

    constexpr VarDef &GetVar() const noexcept {
        return *x_pVar;
    }

    constexpr void SetVar(VarDef &vVar) noexcept {
        x_pVar = &vVar;
    }

private:
    std::unique_ptr<ExprBase> x_upExpr;
    std::string x_sName;
    VarDef *x_pVar;
};

}

#endif
