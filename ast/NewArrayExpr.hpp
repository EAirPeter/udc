#ifndef UDC_AST_NEW_ARRAY_EXPR_HPP_
#define UDC_AST_NEW_ARRAY_EXPR_HPP_

#include <memory>

#include "ExprBase.hpp"

namespace udc::ast {

class NewArrayExpr : public ExprBase {
public:
    NewArrayExpr(
        const location &loc,
        std::unique_ptr<TypeName> &&upType,
        std::unique_ptr<ExprBase> &&upExpr
    ) noexcept;
    virtual ~NewArrayExpr();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }

    inline TypeName *GetTypeName() const noexcept {
        return x_upType.get();
    }

    inline ExprBase *GetExpr() const noexcept {
        return x_upExpr.get();
    }

private:
    std::unique_ptr<TypeName> x_upType;
    std::unique_ptr<ExprBase> x_upExpr;
};

}

#endif
