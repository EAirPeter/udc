#ifndef UDC_AST_NEW_BOOL_LIT_HPP_
#define UDC_AST_NEW_BOOL_LIT_HPP_

#include "ExprBase.hpp"

namespace udc::ast {

class BoolLit : public ExprBase {
public:
    BoolLit(const Location &vLocation, bool bVal) noexcept;
    virtual ~BoolLit();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }

    constexpr bool GetVal() const noexcept {
        return x_bVal;
    }

private:
    bool x_bVal;
};

}

#endif
