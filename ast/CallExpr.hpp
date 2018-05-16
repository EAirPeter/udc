#ifndef UDC_AST_CALL_EXPR_HPP_
#define UDC_AST_CALL_EXPR_HPP_

#include <memory>
#include <vector>

#include "ExprBase.hpp"

namespace udc::ast {

class CallExpr : public ExprBase {
public:
    CallExpr(
        const Location &vLocation,
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

    constexpr bool IsVirtual() const noexcept {
        return ~x_idx;
    }

    constexpr const FnDef &GetFn() const noexcept {
        return *x_pFn;
    }

    constexpr std::size_t GetIdx() const noexcept {
        return x_idx;
    }

    constexpr void SetCallee(const FnDef &vFn, std::size_t idx) noexcept {
        x_pFn = &vFn;
        x_idx = idx;
    }

private:
    std::unique_ptr<ExprBase> x_upExpr;
    std::string x_sName;
    std::vector<std::unique_ptr<ExprBase>> x_vecArgs;
    const FnDef *x_pFn = nullptr;
    std::size_t x_idx = ~std::size_t {};
};

}

#endif
