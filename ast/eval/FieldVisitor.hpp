#ifndef UDC_AST_EVAL_FIELD_VISITOR_HPP_
#define UDC_AST_EVLA_FIELD_VISITOR_HPP_

#include "Fwd.hpp"
#include "VisitorBase.hpp"

namespace udc::ast::eval {

class FieldVisitor : public VisitorBase {
public:
    using VisitorBase::VisitorBase;
    virtual ~FieldVisitor() = default;

    virtual void Visit(Program &vProg) noexcept override;
    virtual void Visit(ClassDef &vClass) noexcept override;
    virtual void Visit(FnDef &vFn) noexcept override;
    virtual void Visit(VarDef &vVar) noexcept override;
    virtual void Visit(TypeName &vTypeName) noexcept override;

private:
    TypeRegistry *x_pTyReg = nullptr;
    ClassTable *x_pstClass = nullptr;
    FnTable *x_pstFn = nullptr;
    std::vector<VarDef *> *x_pVars = nullptr;
    VarTable *x_pstVar = nullptr;
    VfTable *x_pstVf = nullptr;
    ClassDef *x_pClass = nullptr;
    bool x_bPar = false;
    const Type *x_pty = nullptr;
};

}

#endif
