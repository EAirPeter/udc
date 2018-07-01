#ifndef UDC_AST_ARRAY_ACCESS_HPP_
#define UDC_AST_ARRAY_ACCESS_HPP_

#include <memory>

#include "ExprBase.hpp"

namespace udc::ast {

class ArrayAccess final : public ExprBase {
public:
    ArrayAccess(
        const location &loc,
        std::unique_ptr<ExprBase> &&upExpr,
        std::unique_ptr<ExprBase> &&upSub
    ) noexcept;
    virtual ~ArrayAccess();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }

    inline ExprBase *GetExpr() const noexcept {
        return x_upExpr.get();
    }

    inline ExprBase *GetSub() const noexcept {
        return x_upSub.get();
    }


private:
    std::unique_ptr<ExprBase> x_upExpr;
    std::unique_ptr<ExprBase> x_upSub;
};

}

#endif
