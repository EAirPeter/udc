#ifndef UDC_AST_READ_INTEGER_HPP_
#define UDC_AST_READ_INTEGER_HPP_

#include "ExprBase.hpp"

namespace udc::ast {

class ReadInteger : public ExprBase {
public:
    ReadInteger(Driver &drv, const Location &loc) noexcept;
    virtual ~ReadInteger();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }

};

}

#endif
