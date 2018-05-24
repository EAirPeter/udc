#include <llvm/IR/Constants.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>

#include "../Driver.hpp"
#include "../ScopeHelper.hpp"
#include "../ast/All.hpp"
#include "CodeGenVisitor.hpp"

namespace udc::cg {

CodeGenVisitor::CodeGenVisitor(Driver &drv) noexcept : VisitorBase(drv) {}

void CodeGenVisitor::Visit(Program &vProg) noexcept {
    ENTER_SCOPE(x_pTyReg, &vProg.GetTyReg());
    ENTER_SCOPE(x_plvMod, vProg.GetLvMod());
    // decl CRT
    {
        auto ty = llvm::FunctionType::get(y_drv.tyVoidPtr, {y_drv.tySize, y_drv.tySize}, false);
        x_plvCrtCalloc = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("calloc", ty));
        x_plvCrtCalloc->setDSOLocal(true);
        x_plvCrtCalloc->setReturnDoesNotAlias();
    }
    for (auto &upClass : vProg.GetClasses())
        upClass->AcceptVisitor(*this);
}

void CodeGenVisitor::Visit(ClassDef &vClass) noexcept {
    ENTER_SCOPE(x_pClass, &vClass);
    for (auto &upField : vClass.GetFields())
        if (auto pFn = dynamic_cast<FnDef *>(upField.get()))
            pFn->AcceptVisitor(*this);
}

void CodeGenVisitor::Visit(FnDef &vFn) noexcept {
    llvm::Function *fn = vFn.GetLvFn();
    auto bb = llvm::BasicBlock::Create(y_drv.lvCtx, "#fn-entry", fn);
    x_lvBld.SetInsertPoint(bb);
    for (auto &upPar : vFn.GetPars())
        upPar->AcceptVisitor(*this);
    auto arg = fn->arg_begin();
    if (!vFn.IsStatic())
        ++arg;
    for (auto &upPar : vFn.GetPars()) {
        auto ptr = upPar->GetLvPtr();
        x_lvBld.CreateStore(arg++, ptr);
    }
    if (vFn.IsStatic())
        vFn.GetBody()->AcceptVisitor(*this);
    else {
        ENTER_SCOPE(x_plvThis, fn->arg_begin());
        vFn.GetBody()->AcceptVisitor(*this);
    }
}

void CodeGenVisitor::Visit(VarDef &vVar) noexcept {
    assert(!vVar.IsField());
    auto ptr = x_lvBld.CreateAlloca(vVar.GetType().GetLvType(), nullptr, vVar.GetName());
    vVar.SetLvPtr(ptr);
}

void CodeGenVisitor::Visit(BlockStmt &stmt) noexcept {
    for (auto &upItem : stmt.GetItems())
        upItem->AcceptVisitor(*this);
}

void CodeGenVisitor::Visit(BreakStmt &stmt) noexcept {
    assert(x_plvPost);
    x_lvBld.CreateBr(x_plvPost);
}

void CodeGenVisitor::Visit(ExprStmt &stmt) noexcept {
    if (stmt.GetExpr())
        stmt.GetExpr()->AcceptVisitor(*this);
}

void CodeGenVisitor::Visit(ForStmt &stmt) noexcept {
    auto fn = x_lvBld.GetInsertBlock()->getParent();
    auto bbCond = llvm::BasicBlock::Create(y_drv.lvCtx, "#for-cond", fn);
    auto bbBody = llvm::BasicBlock::Create(y_drv.lvCtx, "#for-body", fn);
    auto bbPost = llvm::BasicBlock::Create(y_drv.lvCtx, "#for-post", fn);
    if (stmt.GetInit())
        stmt.GetInit()->AcceptVisitor(*this);
    x_lvBld.CreateBr(bbCond);
    x_lvBld.SetInsertPoint(bbCond);
    if (stmt.GetCond()) {
        stmt.GetCond()->AcceptVisitor(*this);
        x_lvBld.CreateCondBr(x_plvRet, bbBody, bbPost);
    }
    else
        x_lvBld.CreateBr(bbBody);
    x_lvBld.SetInsertPoint(bbBody);
    ENTER_SCOPE(x_plvPost, bbPost);
    stmt.GetBody()->AcceptVisitor(*this);
    if (stmt.GetIncr())
        stmt.GetIncr()->AcceptVisitor(*this);
    x_lvBld.CreateBr(bbCond);
    x_lvBld.SetInsertPoint(bbPost);
}

void CodeGenVisitor::Visit(IfStmt &stmt) noexcept {
    auto fn = x_lvBld.GetInsertBlock()->getParent();
    auto bbThen = llvm::BasicBlock::Create(y_drv.lvCtx, "#if-then", fn);
    auto bbElse = llvm::BasicBlock::Create(y_drv.lvCtx, "#if-else", fn);
    auto bbPost = llvm::BasicBlock::Create(y_drv.lvCtx, "#if-post", fn);
    stmt.GetCond()->AcceptVisitor(*this);
    x_lvBld.CreateCondBr(x_plvRet, bbThen, bbElse);
    x_lvBld.SetInsertPoint(bbThen);
    stmt.GetThen()->AcceptVisitor(*this);
    x_lvBld.CreateBr(bbPost);
    x_lvBld.SetInsertPoint(bbElse);
    if (stmt.GetElse())
        stmt.GetElse()->AcceptVisitor(*this);
    x_lvBld.CreateBr(bbPost);
    x_lvBld.SetInsertPoint(bbPost);
}

void CodeGenVisitor::Visit(PrintStmt &stmt) noexcept {
    // TODO: print stub
    for (auto &upArg : stmt.GetArgs()) {
        upArg->AcceptVisitor(*this);
        if (upArg->GetType() == x_pTyReg->tyInt) {
        }
        else if (upArg->GetType() == x_pTyReg->tyBool) {
        }
        else if (upArg->GetType() == x_pTyReg->tyString) {
        }
        // else wtf
    }
}

void CodeGenVisitor::Visit(ReturnStmt &stmt) noexcept {
    if (stmt.GetExpr()) {
        stmt.GetExpr()->AcceptVisitor(*this);
        x_lvBld.CreateRet(x_lvBld.CreateBitCast(
            x_plvRet, x_lvBld.GetInsertBlock()->getParent()->getReturnType()
        ));
    }
    else {
        x_lvBld.CreateRetVoid();
    }
}

void CodeGenVisitor::Visit(WhileStmt &stmt) noexcept {
    auto fn = x_lvBld.GetInsertBlock()->getParent();
    auto bbCond = llvm::BasicBlock::Create(y_drv.lvCtx, "#while-cond", fn);
    auto bbBody = llvm::BasicBlock::Create(y_drv.lvCtx, "#while-body", fn);
    auto bbPost = llvm::BasicBlock::Create(y_drv.lvCtx, "#while-post", fn);
    x_lvBld.CreateBr(bbCond);
    x_lvBld.SetInsertPoint(bbCond);
    stmt.GetCond()->AcceptVisitor(*this);
    x_lvBld.CreateCondBr(x_plvRet, bbBody, bbPost);
    x_lvBld.SetInsertPoint(bbBody);
    ENTER_SCOPE(x_plvPost, bbPost);
    stmt.GetBody()->AcceptVisitor(*this);
    x_lvBld.CreateBr(bbCond);
    x_lvBld.SetInsertPoint(bbPost);
}

void CodeGenVisitor::Visit(AssignExpr &expr) noexcept {
    expr.GetRhs()->AcceptVisitor(*this);
    auto val = x_plvRet;
    expr.GetLhs()->SetGenPtr(true);
    expr.GetLhs()->AcceptVisitor(*this);
    auto ptr = x_plvRet;
    x_lvBld.CreateStore(x_lvBld.CreateBitCast(val, expr.GetLhs()->GetType().GetLvType()), ptr);
    x_plvRet = val;
}

void CodeGenVisitor::Visit(BinaryExpr &expr) noexcept {
    expr.GetLhs()->AcceptVisitor(*this);
    auto lhs = x_plvRet;
    expr.GetRhs()->AcceptVisitor(*this);
    auto rhs = x_plvRet;
    switch (expr.GetOp()) {
    case BinOp::kIor:
        x_plvRet = x_lvBld.CreateOr(lhs, rhs);
        break;
    case BinOp::kAnd:
        x_plvRet = x_lvBld.CreateAnd(lhs, rhs);
        break;
    case BinOp::kEqu:
        x_plvRet = x_lvBld.CreateICmpEQ(lhs, rhs);
        break;
    case BinOp::kNeq:
        x_plvRet = x_lvBld.CreateICmpNE(lhs, rhs);
        break;
    case BinOp::kLes:
        x_plvRet = x_lvBld.CreateICmpSLT(lhs, rhs);
        break;
    case BinOp::kGre:
        x_plvRet = x_lvBld.CreateICmpSGT(lhs, rhs);
        break;
    case BinOp::kLeq:
        x_plvRet = x_lvBld.CreateICmpSLE(lhs, rhs);
        break;
    case BinOp::kGeq:
        x_plvRet = x_lvBld.CreateICmpSGE(lhs, rhs);
        break;
    case BinOp::kAdd:
        x_plvRet = x_lvBld.CreateAdd(lhs, rhs, {}, false, true);
        break;
    case BinOp::kSub:
        x_plvRet = x_lvBld.CreateSub(lhs, rhs, {}, false, true);
        break;
    case BinOp::kMul:
        x_plvRet = x_lvBld.CreateMul(lhs, rhs, {}, false, true);
        break;
    case BinOp::kDiv:
        x_plvRet = x_lvBld.CreateSDiv(lhs, rhs);
        break;
    case BinOp::kMod:
        x_plvRet = x_lvBld.CreateSRem(lhs, rhs);
        break;
    }
}

void CodeGenVisitor::Visit(CallExpr &expr) noexcept {
    if (expr.GetExpr())
        expr.GetExpr()->AcceptVisitor(*this);
    else
        x_plvRet = x_plvThis;
    auto who = x_plvRet;
    auto &vFn = expr.GetFn();
    llvm::SmallVector<llvm::Value *, 64> vec;
    if (!vFn.IsStatic())
        vec.emplace_back(x_plvThis);
    for (auto &upArg : expr.GetArgs()) {
        upArg->AcceptVisitor(*this);
        vec.emplace_back(x_plvRet);
    }
    if (vFn.IsStatic())
        x_plvRet = x_lvBld.CreateCall(vFn.GetLvFn(), vec);
    else {
        // virtual call
        // vtable = *(void ***) who
        // pfn = vtable[idx + 1]
        // ((ret_ty (*)(arg_tys...)) pfn)(args...)
        auto i8p = x_lvBld.CreateLoad(x_lvBld.CreateBitCast(who, y_drv.tyVoidPtrPtr->getPointerTo()));
        auto idx = llvm::ConstantInt::get(y_drv.tySize, vFn.GetIdx() + 1);
        auto pfn = x_lvBld.CreateLoad(x_lvBld.CreateGEP(i8p, idx));
        x_plvRet = x_lvBld.CreateCall(vFn.GetLvFn()->getFunctionType(), pfn, vec);
    }
}

void CodeGenVisitor::Visit(CastExpr &expr) noexcept {
    expr.GetExpr()->AcceptVisitor(*this);
    if (expr.GetType().Accepts(expr.GetExpr()->GetType())) {
        // static cast
        x_plvRet = x_lvBld.CreateBitCast(x_plvRet, expr.GetType().GetLvType());
        return;
    }
    // dynamic cast
    // TODO: dynamic cast stub
}

void CodeGenVisitor::Visit(NewArrayExpr &expr) noexcept {
    expr.GetExpr()->AcceptVisitor(*this);
    auto count = x_plvRet;
    // TODO: check count non-negative, record length
    auto ty = x_pTyReg->DecDim(expr.GetType()).GetLvType();
    auto size = llvm::ConstantInt::get(y_drv.tySize, y_drv.lvDataLayout.getTypeAllocSize(ty));
    auto res = x_lvBld.CreateCall(x_plvCrtCalloc, {count, size});
    x_plvRet = x_lvBld.CreateBitCast(res, expr.GetType().GetLvType());
}

void CodeGenVisitor::Visit(NewClassExpr &expr) noexcept {
    auto count = llvm::ConstantInt::get(y_drv.tySize, 1);
    auto pClass = static_cast<const ClassDef *>(&expr.GetType().GetElemType());
    auto size = llvm::ConstantInt::get(y_drv.tySize, y_drv.lvDataLayout.getTypeAllocSize(pClass->GetLvType()));
    auto res = x_lvBld.CreateCall(x_plvCrtCalloc, {count, size});
    x_plvRet = x_lvBld.CreateBitCast(res, expr.GetType().GetLvType());
}

void CodeGenVisitor::Visit(UnaryExpr &expr) noexcept {
    expr.GetExpr()->AcceptVisitor(*this);
    switch (expr.GetOp()) {
    case UnaOp::kNeg:
        x_plvRet = x_lvBld.CreateNeg(x_plvRet, {}, false, true);
        break;
    case UnaOp::kNot:
        x_plvRet = x_lvBld.CreateNot(x_plvRet);
        break;
    }
}

void CodeGenVisitor::Visit(ArrayAccess &expr) noexcept {
    expr.GetExpr()->AcceptVisitor(*this);
    auto ptr = x_plvRet;
    expr.GetSub()->AcceptVisitor(*this);
    auto sub = x_plvRet;
    // TODO: check bound
}

void CodeGenVisitor::Visit(InstanceOf &expr) noexcept {
}

void CodeGenVisitor::Visit(ReadInteger &expr) noexcept {
}

void CodeGenVisitor::Visit(ReadLine &expr) noexcept {
}

void CodeGenVisitor::Visit(This &expr) noexcept {
}

void CodeGenVisitor::Visit(VarAccess &expr) noexcept {
}

void CodeGenVisitor::Visit(BoolLit &expr) noexcept {
}

void CodeGenVisitor::Visit(IntLit &expr) noexcept {
}

void CodeGenVisitor::Visit(NullLit &expr) noexcept {
}

void CodeGenVisitor::Visit(StrLit &expr) noexcept {
}

}
