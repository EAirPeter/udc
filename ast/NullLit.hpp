#ifndef UDC_AST_NULL_LIT_HPP_
#define UDC_AST_NULL_LIT_HPP_

#include "ExprBase.hpp"

namespace udc::ast {

class NullLit : public ExprBase {
public:
    NullLit(Driver &drv, const Location &loc) noexcept;
    virtual ~NullLit();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;
    
    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }
};

}

#endif
