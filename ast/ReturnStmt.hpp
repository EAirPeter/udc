#ifndef UDC_AST_RETURN_STMT_HPP_
#define UDC_AST_RETURN_STMT_HPP_

#include <memory>

#include "NodeBase.hpp"

namespace udc::ast {

class ReturnStmt : public NodeBase {
public:
    ReturnStmt(const Location &vLocation, std::unique_ptr<ExprBase> &&upExpr) noexcept;
    virtual ~ReturnStmt();

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
