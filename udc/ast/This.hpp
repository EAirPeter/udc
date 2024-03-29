#ifndef UDC_AST_THIS_HPP_
#define UDC_AST_THIS_HPP_

#include "ExprBase.hpp"

namespace udc::ast {

class This final : public ExprBase {
public:
    This(const location &loc) noexcept;
    virtual ~This();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }

};

}

#endif
