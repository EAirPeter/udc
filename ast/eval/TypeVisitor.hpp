#ifndef UDC_AST_EVAL_TYPE_VISITOR_HPP_
#define UDC_AST_EVLA_TYPE_VISITOR_HPP_

#include "Fwd.hpp"
#include "VisitorBase.hpp"

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
    
    virtual void Visit(AssignExpr &expr) noexcept override;
    virtual void Visit(BinaryExpr &expr) noexcept override;
    virtual void Visit(CallExpr &expr) noexcept override;
    virtual void Visit(CastExpr &expr) noexcept override;
    virtual void Visit(NewArrayExpr &expr) noexcept override;
    virtual void Visit(NewClassExpr &expr) noexcept override;
    virtual void Visit(UnaryExpr &expr) noexcept override;

    virtual void Visit(ArrayAccess &expr) noexcept override;
    virtual void Visit(InstanceOf &expr) noexcept override;
    virtual void Visit(ReadInteger &expr) noexcept override;
    virtual void Visit(ReadLine &expr) noexcept override;
    virtual void Visit(This &expr) noexcept override;
    virtual void Visit(VarAccess &expr) noexcept override;

    virtual void Visit(BoolLit &expr) noexcept override;
    virtual void Visit(IntLit &expr) noexcept override;
    virtual void Visit(NullLit &expr) noexcept override;
    virtual void Visit(StrLit &expr) noexcept override;

    virtual void Visit(TypeName &vTypeName) noexcept override;

private:
    TypeRegistry *x_pTyReg = nullptr;
    ClassTable *x_pstClass = nullptr;
    FnTable *x_pstFn = nullptr;
    VarTable *x_pstVar = nullptr;
    VfTable *x_pstVf = nullptr;
    ClassDef *x_pClass = nullptr;
    FnDef *x_pFn = nullptr;
    NodeBase *x_pLoop = nullptr;
    const Type *x_pty = nullptr;
    FnDef *x_pMain = nullptr;
};

}

#endif
