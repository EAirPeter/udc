#ifndef UDC_AST_READ_LINE_HPP_
#define UDC_AST_READ_LINE_HPP_

#include "ExprBase.hpp"

namespace udc::ast {

class ReadLine final : public ExprBase {
public:
    ReadLine(const location &loc) noexcept;
    virtual ~ReadLine();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }

};

}

#endif
