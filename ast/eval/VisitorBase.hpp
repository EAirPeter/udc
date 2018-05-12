#ifndef UDC_AST_EVAL_VISITOR_BASE_HPP_
#define UDC_AST_EVAL_VISITOR_BASE_HPP_

#include "../Fwd.hpp"

namespace udc::ast::eval {

#define DEF_VISIT(type_) virtual inline void Visit(type_ &) {}
struct VisitorBase {
    virtual inline ~VisitorBase() = default;
    DEF_VISIT(NodeBase)
    DEF_VISIT(ClassDef)
    DEF_VISIT(FnDef)
    DEF_VISIT(Program)
    DEF_VISIT(TypeName)
    DEF_VISIT(VarDef)

    DEF_VISIT(BlockStmt)
    DEF_VISIT(BreakStmt)
    DEF_VISIT(ExprStmt)
    DEF_VISIT(ForStmt)
    DEF_VISIT(IfStmt)
    DEF_VISIT(PrintStmt)
    DEF_VISIT(ReturnStmt)
    DEF_VISIT(WhileStmt)

    DEF_VISIT(AssignExpr)
    DEF_VISIT(BinaryExpr)
    DEF_VISIT(CallExpr)
    DEF_VISIT(CastExpr)
    DEF_VISIT(NewArrayExpr)
    DEF_VISIT(NewClassExpr)
    DEF_VISIT(UnaryExpr)

    DEF_VISIT(ArrayAccess)
    DEF_VISIT(InstanceOf)
    DEF_VISIT(ReadInteger)
    DEF_VISIT(ReadLine)
    DEF_VISIT(This)
    DEF_VISIT(VarAccess)

    DEF_VISIT(BoolLit)
    DEF_VISIT(IntLit)
    DEF_VISIT(NullLit)
    DEF_VISIT(StrLit)
};
#undef DEF_VISIT

}

#endif
