#ifndef UDC_AST_CALL_EXPR_HPP_
#define UDC_AST_CALL_EXPR_HPP_

#include <memory>
#include <vector>

#include "ExprBase.hpp"

namespace udc::ast {

class CallExpr : public ExprBase {
public:
    CallExpr(
        Driver &drv,
        const Location &loc,
        std::unique_ptr<ExprBase> &&upExpr,
        std::string &&sName,
        std::vector<std::unique_ptr<ExprBase>> &&vecArgs
    ) noexcept;
    virtual ~CallExpr();

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

    constexpr const std::vector<std::unique_ptr<ExprBase>> &GetArgs() const noexcept {
        return x_vecArgs;
    }

    constexpr const FnDef &GetFn() const noexcept {
        return *x_pFn;
    }

    constexpr void SetFn(const FnDef &vFn) noexcept {
        x_pFn = &vFn;
    }

private:
    std::unique_ptr<ExprBase> x_upExpr;
    std::string x_sName;
    std::vector<std::unique_ptr<ExprBase>> x_vecArgs;
    const FnDef *x_pFn = nullptr;
};

}

#endif
