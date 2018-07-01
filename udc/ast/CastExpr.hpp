#ifndef UDC_AST_CAST_EXPR_HPP_
#define UDC_AST_CAST_EXPR_HPP_

#include <memory>

#include "ExprBase.hpp"

namespace udc::ast {

class CastExpr final : public ExprBase {
public:
    CastExpr(
        const location &loc,
        std::string &&sName,
        std::unique_ptr<ExprBase> &&upExpr
    ) noexcept;
    virtual ~CastExpr();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }

    constexpr const std::string &GetName() const noexcept {
        return x_sName;
    }

    inline ExprBase *GetExpr() const noexcept {
        return x_upExpr.get();
    }

private:
    std::string x_sName;
    std::unique_ptr<ExprBase> x_upExpr;
};

}

#endif
