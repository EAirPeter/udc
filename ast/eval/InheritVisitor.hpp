#ifndef UDC_AST_EVAL_INHERENCE_VISITOR_HPP_
#define UDC_AST_EVLA_INHERENCE_VISITOR_HPP_

#include "Fwd.hpp"
#include "VisitorBase.hpp"

namespace udc::ast::eval {

class InheritVisitor : public VisitorBase {
public:
    using VisitorBase::VisitorBase;
    virtual inline ~InheritVisitor() = default;

    virtual void Visit(Program &vProg) noexcept override;
    virtual void Visit(ClassDef &vClass) noexcept override;

private:
    const ClassTable *x_pstClass = nullptr;
};

}

#endif
