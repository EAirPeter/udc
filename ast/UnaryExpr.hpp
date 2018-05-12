#ifndef UDC_AST_UNARY_EXPR_HPP_
#define UDC_AST_UNARY_EXPR_HPP_

#include <memory>

#include "ExprBase.hpp"

namespace udc::ast {

enum class UnaOp {
    kNeg,
    kNot,
};

class UnaryExpr : public ExprBase {
public:
    UnaryExpr(
        const Location &vLocation,
        UnaOp vOp,
        std::unique_ptr<ExprBase> &&upExpr
    ) noexcept;
    virtual ~UnaryExpr();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }

    constexpr UnaOp GetOp() const noexcept {
        return x_vOp;
    }

    inline ExprBase *GetExpr() const noexcept {
        return x_upExpr.get();
    }

private:
    UnaOp x_vOp;
    std::unique_ptr<ExprBase> x_upExpr;
};

}

#endif
