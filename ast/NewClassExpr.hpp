#ifndef UDC_AST_NEW_CLASS_EXPR_HPP_
#define UDC_AST_NEW_CLASS_EXPR_HPP_

#include "ExprBase.hpp"

namespace udc::ast {

class NewClassExpr : public ExprBase {
public:
    NewClassExpr(
        const location &loc,
        std::string &&sName
    ) noexcept;
    virtual ~NewClassExpr();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }

    constexpr const std::string &GetName() const noexcept {
        return x_sName;
    }

private:
    std::string x_sName;
};

}

#endif
