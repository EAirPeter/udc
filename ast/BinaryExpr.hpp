#ifndef UDC_AST_BINARY_EXPR_HPP_
#define UDC_AST_BINARY_EXPR_HPP_

#include <memory>

#include "ExprBase.hpp"

namespace udc::ast {

enum class BinOp {
    kIor,
    kAnd,
    kEqu,
    kNeq,
    kLes,
    kGre,
    kLeq,
    kGeq,
    kAdd,
    kSub,
    kMul,
    kDiv,
    kMod,
};

class BinaryExpr : public ExprBase {
public:
    BinaryExpr(
        Driver &drv,
        const Location &loc,
        BinOp vOp,
        std::unique_ptr<ExprBase> &&upLhs,
        std::unique_ptr<ExprBase> &&upRhs
    ) noexcept;
    virtual ~BinaryExpr();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }

    constexpr BinOp GetOp() const noexcept {
        return x_vOp;
    }

    inline ExprBase *GetLhs() const noexcept {
        return x_upLhs.get();
    }

    inline ExprBase *GetRhs() const noexcept {
        return x_upRhs.get();
    }

private:
    BinOp x_vOp;
    std::unique_ptr<ExprBase> x_upLhs;
    std::unique_ptr<ExprBase> x_upRhs;
};

}

#endif
