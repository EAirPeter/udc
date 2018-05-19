#ifndef UDC_AST_ASSIGN_EXPR_HPP_
#define UDC_AST_ASSIGN_EXPR_HPP_

#include <memory>

#include "ExprBase.hpp"

namespace udc::ast {

class AssignExpr : public ExprBase {
public:
    AssignExpr(
        Driver &drv,
        const Location &loc,
        std::unique_ptr<ExprBase> &&upLhs,
        std::unique_ptr<ExprBase> &&upRhs
    ) noexcept;
    virtual ~AssignExpr();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }

    inline ExprBase *GetLhs() const noexcept {
        return x_upLhs.get();
    }

    inline ExprBase *GetRhs() const noexcept {
        return x_upRhs.get();
    }

private:
    std::unique_ptr<ExprBase> x_upLhs;
    std::unique_ptr<ExprBase> x_upRhs;
};

}

#endif
