#ifndef UDC_CG_DYNAMIC_GEN_VISITOR_HPP_
#define UDC_CG_DYNAMIC_GEN_VISITOR_HPP_

#include "../ast/Fwd.hpp"
#include "../ast/eval/VisitorBase.hpp"

namespace llvm {
class Module;
}

namespace udc {
class Driver;
}

namespace udc::cg {

using namespace udc::ast;
using namespace udc::ast::eval;

class DynamicGenVisitor : public VisitorBase {
public:
    DynamicGenVisitor(Driver &drv) noexcept;
    virtual inline ~DynamicGenVisitor() = default;

public:
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

private:
    llvm::Module *x_plvMod = nullptr;
    ClassDef *x_pClass = nullptr;
};

}

#endif
