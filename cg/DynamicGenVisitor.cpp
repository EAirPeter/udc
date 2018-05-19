#include <llvm/IR/IRBuilder.h>

#include "../Driver.hpp"
#include "../ScopeHelper.hpp"
#include "../ast/All.hpp"
#include "DynamicGenVisitor.hpp"

namespace udc::cg {

DynamicGenVisitor::DynamicGenVisitor(Driver &drv) noexcept : VisitorBase(drv) {}

void DynamicGenVisitor::Visit(Program &vProg) noexcept {
    ENTER_SCOPE(x_plvMod, vProg.GetLvMod());
    for (auto &upClass : vProg.GetClasses())
        upClass->AcceptVisitor(*this);
}

void DynamicGenVisitor::Visit(ClassDef &vClass) noexcept {
    ENTER_SCOPE(x_pClass, &vClass);
    for (auto &upField : vClass.GetFields())
        upField->AcceptVisitor(*this);
}

void DynamicGenVisitor::Visit(FnDef &vFn) noexcept {

    //auto pFn = x_plvMod->getOrInsertFunction(x_pClass->GetName() + '.' + vFn.GetName(), nullptr);
}

void DynamicGenVisitor::Visit(VarDef &vVar) noexcept {
    // do nothing
}

void DynamicGenVisitor::Visit(BlockStmt &stmt) noexcept {}
void DynamicGenVisitor::Visit(BreakStmt &stmt) noexcept {}
void DynamicGenVisitor::Visit(ExprStmt &stmt) noexcept {}
void DynamicGenVisitor::Visit(ForStmt &stmt) noexcept {}
void DynamicGenVisitor::Visit(IfStmt &stmt) noexcept {}
void DynamicGenVisitor::Visit(PrintStmt &stmt) noexcept {}
void DynamicGenVisitor::Visit(ReturnStmt &stmt) noexcept {}
void DynamicGenVisitor::Visit(WhileStmt &stmt) noexcept {}
    
void DynamicGenVisitor::Visit(AssignExpr &expr) noexcept {}
void DynamicGenVisitor::Visit(BinaryExpr &expr) noexcept {}
void DynamicGenVisitor::Visit(CallExpr &expr) noexcept {}
void DynamicGenVisitor::Visit(CastExpr &expr) noexcept {}
void DynamicGenVisitor::Visit(NewArrayExpr &expr) noexcept {}
void DynamicGenVisitor::Visit(NewClassExpr &expr) noexcept {}
void DynamicGenVisitor::Visit(UnaryExpr &expr) noexcept {}

void DynamicGenVisitor::Visit(ArrayAccess &expr) noexcept {}
void DynamicGenVisitor::Visit(InstanceOf &expr) noexcept {}
void DynamicGenVisitor::Visit(ReadInteger &expr) noexcept {}
void DynamicGenVisitor::Visit(ReadLine &expr) noexcept {}
void DynamicGenVisitor::Visit(This &expr) noexcept {}
void DynamicGenVisitor::Visit(VarAccess &expr) noexcept {}

void DynamicGenVisitor::Visit(BoolLit &expr) noexcept {}
void DynamicGenVisitor::Visit(IntLit &expr) noexcept {}
void DynamicGenVisitor::Visit(NullLit &expr) noexcept {}
void DynamicGenVisitor::Visit(StrLit &expr) noexcept {}

}
