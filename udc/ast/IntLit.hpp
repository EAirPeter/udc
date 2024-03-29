#ifndef UDC_AST_INT_LIT_HPP_
#define UDC_AST_INT_LIT_HPP_

#include "ExprBase.hpp"

namespace udc::ast {

class IntLit final : public ExprBase {
public:
    IntLit(const location &loc, int nVal) noexcept;
    virtual ~IntLit();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }

    constexpr int GetVal() const noexcept {
        return x_nVal;
    }

private:
    int x_nVal;
};

}

#endif
