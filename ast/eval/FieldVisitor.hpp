#ifndef UDC_AST_EVAL_FIELD_VISITOR_HPP_
#define UDC_AST_EVLA_FIELD_VISITOR_HPP_

#include "Fwd.hpp"
#include "VisitorBase.hpp"

namespace udc {
class Driver;
}

namespace udc::ast::eval {

class FieldVisitor : public VisitorBase {
public:
    using VisitorBase::VisitorBase;
    virtual inline ~FieldVisitor() = default;

    virtual void Visit(Program &vProg) noexcept override;
    virtual void Visit(ClassDef &vClass) noexcept override;
    virtual void Visit(FnDef &vFn) noexcept override;
    virtual void Visit(VarDef &vVar) noexcept override;
    virtual void Visit(TypeName &vTypeName) noexcept override;

private:
    ClassTable *x_pstClass = nullptr;
    FnTable *x_pstFn = nullptr;
    VarTable *x_pstVar = nullptr;
    Type x_ty;
};

}

#endif
