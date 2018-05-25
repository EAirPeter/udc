#ifndef UDC_AST_EXPR_STMT_HPP_
#define UDC_AST_EXPR_STMT_HPP_

#include <memory>

#include "NodeBase.hpp"

namespace udc::ast {

class ExprStmt final : public NodeBase {
public:
    ExprStmt(const location &loc, std::unique_ptr<ExprBase> &&upExpr) noexcept;
    virtual ~ExprStmt();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;
    
    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }

    inline ExprBase *GetExpr() const noexcept {
        return x_upExpr.get();
    }

private:
    std::unique_ptr<ExprBase> x_upExpr;
};

}

#endif
