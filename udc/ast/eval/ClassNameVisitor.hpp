#ifndef UDC_AST_EVAL_CLASS_NAME_VISITOR_HPP_
#define UDC_AST_EVLA_CLASS_NAME_VISITOR_HPP_

#include "Fwd.hpp"
#include "VisitorBase.hpp"

namespace udc::ast::eval {

class ClassNameVisitor final : public VisitorBase {
public:
    using VisitorBase::VisitorBase;
    virtual ~ClassNameVisitor() = default;

    virtual void Visit(Program &vProg) noexcept override;
    virtual void Visit(ClassDef &vClass) noexcept override;

private:
    TypeRegistry *x_pTyReg = nullptr;
    ClassTable *x_pstClass = nullptr;
};

}

#endif
