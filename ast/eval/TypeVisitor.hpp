#ifndef UDC_AST_EVAL_TYPE_VISITOR_HPP_
#define UDC_AST_EVLA_TYPE_VISITOR_HPP_

#include "Fwd.hpp"
#include "VisitorBase.hpp"

namespace udc {
class Driver;
}

namespace udc::ast::eval {

class TypeVisitor : public VisitorBase {
public:
    using VisitorBase::VisitorBase;
    virtual inline ~TypeVisitor() = default;

    virtual void Visit(Program &vProg) noexcept override;

    virtual void Visit(ClassDef &vClass) noexcept override;
    virtual void Visit(FnDef &vFn) noexcept override;
    virtual void Visit(VarDef &vVar) noexcept override;

    virtual void Visit(BlockStmt &stmt) noexcept override;
    virtual void Visit(BreakStmt &stmt) noexcept override;
    virtual void Visit(ExprStmt &stmt) noexcept override;
    virtual void Visit(ForStmt &stmt) noexcept override;
    virtual void Visit(IfStmt &stmt) noexcept override;
    virtual void Visit(PrintStmt &stmt) noexcept override;
    virtual void Visit(ReturnStmt &stmt) noexcept override;
    virtual void Visit(WhileStmt &stmt) noexcept override;
    
    virtual void Visit(TypeName &vTypeName) noexcept override;

private:
    ClassTable *x_pstClass = nullptr;
    FnTable *x_pstFn = nullptr;
    VarTable *x_pstVar = nullptr;
    FnDef *x_pFn = nullptr;
    NodeBase *x_pLoop = nullptr;
    Type x_ty;
};

}

#endif
