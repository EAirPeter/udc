#include <llvm/IR/CFG.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Utils.h>

#include "../Print.hpp"
#include "../ScopeHelper.hpp"
#include "../ast/All.hpp"
#include "CGContext.hpp"
#include "CodeGenVisitor.hpp"

namespace udc::cg {

CodeGenVisitor::CodeGenVisitor(CGContext &ctx, bool bOptimiz) noexcept : x_ctx(ctx), x_lvBld(x_ctx.lvCtx), x_bOptimiz(bOptimiz) {}

inline llvm::Constant *CodeGenVisitor::X_EmitStringLiteral(const char *psz) noexcept {
    assert(x_plvMod);
    auto it = x_mapStrLits.find(psz);
    if (it != x_mapStrLits.end())
        return it->second;
    auto len = std::strlen(psz) + 1;
    auto ty = llvm::ArrayType::get(x_ctx.tyI8, len);
    auto gv = llvm::cast<llvm::GlobalVariable>(x_plvMod->getOrInsertGlobal(FmtStr("#StrLit.%zu", x_mapStrLits.size()), ty));
    gv->setAlignment(1);
    gv->setConstant(true);
    gv->setDSOLocal(true);
    gv->setLinkage(llvm::GlobalValue::InternalLinkage);
    llvm::SmallVector<llvm::Constant *, 64> vec;
    for (std::size_t i = 0; i < len; ++i)
        vec.emplace_back(llvm::ConstantInt::get(x_ctx.tyI8, psz[i]));
    gv->setInitializer(llvm::ConstantArray::get(ty, vec));
    auto res = llvm::ConstantExpr::getPointerCast(gv, x_ctx.tyI8Ptr);
    x_mapStrLits.emplace(std::piecewise_construct, std::forward_as_tuple(psz), std::forward_as_tuple(res));
    return res;
}

void CodeGenVisitor::Visit(Program &vProg) noexcept {
    ENTER_SCOPE(x_pTyReg, &vProg.GetTyReg());
    ENTER_SCOPE(x_plvMod, vProg.GetLvMod());
    ENTER_SCOPE(x_plvClassIdx, vProg.GetLvClassIdx());
    llvm::legacy::FunctionPassManager fpm(x_plvMod);
    if (x_bOptimiz) {
        fpm.add(llvm::createPromoteMemoryToRegisterPass());
        fpm.add(llvm::createInstructionCombiningPass());
        fpm.add(llvm::createReassociatePass());
        fpm.add(llvm::createGVNPass());
        fpm.add(llvm::createCFGSimplificationPass());
    }
    fpm.doInitialization();
    ENTER_SCOPE(x_plvFpm, &fpm);
    {
        llvm::FunctionType *ty;
        // void UdcRtlPrintf(const char *fmt, ...);
        ty = llvm::FunctionType::get(x_ctx.tyVoid, x_ctx.tyI8Ptr, true);
        x_plvRtlPrintf = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("UdcRtlPrintf", ty));
        x_plvRtlPrintf->setCallingConv(llvm::CallingConv::C);
        x_plvRtlPrintf->setDSOLocal(true);
        x_plvRtlPrintf->setDoesNotRecurse();
        x_plvRtlPrintf->setDoesNotThrow();
        x_plvRtlPrintf->setLinkage(llvm::GlobalValue::ExternalLinkage);
        // __declspec(noalias) void *UdcRtlAlloc(size_t cb);
        ty = llvm::FunctionType::get(x_ctx.tyVoidPtr, {x_ctx.tySize}, false);
        x_plvRtlAlloc = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("UdcRtlAlloc", ty));
        x_plvRtlAlloc->setCallingConv(llvm::CallingConv::C);
        x_plvRtlAlloc->setDSOLocal(true);
        x_plvRtlAlloc->setDoesNotRecurse();
        x_plvRtlAlloc->setDoesNotThrow();
        x_plvRtlAlloc->setLinkage(llvm::GlobalValue::ExternalLinkage);
        x_plvRtlAlloc->setOnlyAccessesArgMemory();
        x_plvRtlAlloc->setReturnDoesNotAlias();
        // __declspec(noalias) void *UdcRtlReAlloc(void *ptr, size_t cb);
        ty = llvm::FunctionType::get(x_ctx.tyVoidPtr, {x_ctx.tyVoidPtr, x_ctx.tySize}, false);
        x_plvRtlReAlloc = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("UdcRtlReAlloc", ty));
        x_plvRtlReAlloc->setCallingConv(llvm::CallingConv::C);
        x_plvRtlReAlloc->setDSOLocal(true);
        x_plvRtlReAlloc->setDoesNotRecurse();
        x_plvRtlReAlloc->setDoesNotThrow();
        x_plvRtlReAlloc->setLinkage(llvm::GlobalValue::ExternalLinkage);
        x_plvRtlReAlloc->setOnlyAccessesArgMemory();
        x_plvRtlReAlloc->setReturnDoesNotAlias();
        x_plvRtlReAlloc->arg_begin()[0].addAttr(llvm::Attribute::NoCapture);
        // __declspec(noalias) void *UdcRtlAllocArray(int nLen, size_t cbOff, size_t cbElem);
        ty = llvm::FunctionType::get(x_ctx.tyVoidPtr, {x_ctx.tyI32, x_ctx.tySize, x_ctx.tySize}, false);
        x_plvRtlAllocArray = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("UdcRtlAllocArray", ty));
        x_plvRtlAllocArray->setCallingConv(llvm::CallingConv::C);
        x_plvRtlAllocArray->setDSOLocal(true);
        x_plvRtlAllocArray->setDoesNotRecurse();
        x_plvRtlAllocArray->setDoesNotThrow();
        x_plvRtlAllocArray->setLinkage(llvm::GlobalValue::ExternalLinkage);
        x_plvRtlAllocArray->setOnlyAccessesArgMemory();
        x_plvRtlAllocArray->setReturnDoesNotAlias();
        // bool UdcRtlInstanceOf(const void *who, const void *idx);
        ty = llvm::FunctionType::get(x_ctx.tyI1, {x_ctx.tyVoidPtr, x_ctx.tyVoidPtr}, false);
        x_plvRtlInstanceOf = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("UdcRtlInstanceOf", ty));
        x_plvRtlInstanceOf->setCallingConv(llvm::CallingConv::C);
        x_plvRtlInstanceOf->setDSOLocal(true);
        x_plvRtlInstanceOf->setDoesNotRecurse();
        x_plvRtlInstanceOf->setDoesNotThrow();
        x_plvRtlInstanceOf->setLinkage(llvm::GlobalValue::ExternalLinkage);
        x_plvRtlInstanceOf->setOnlyReadsMemory();
        x_plvRtlInstanceOf->arg_begin()[0].addAttr(llvm::Attribute::NoCapture);
        x_plvRtlInstanceOf->arg_begin()[0].addAttr(llvm::Attribute::ReadOnly);
        x_plvRtlInstanceOf->arg_begin()[1].addAttr(llvm::Attribute::ReadNone);
        // void UdcRtlCheckCast(const void *who, const void *idx);
        ty = llvm::FunctionType::get(x_ctx.tyVoid, {x_ctx.tyVoidPtr, x_ctx.tyVoidPtr}, false);
        x_plvRtlCheckCast = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("UdcRtlCheckCast", ty));
        x_plvRtlCheckCast->setCallingConv(llvm::CallingConv::C);
        x_plvRtlCheckCast->setDSOLocal(true);
        x_plvRtlCheckCast->setDoesNotRecurse();
        x_plvRtlCheckCast->setDoesNotThrow();
        x_plvRtlCheckCast->setLinkage(llvm::GlobalValue::ExternalLinkage);
        x_plvRtlCheckCast->arg_begin()[0].addAttr(llvm::Attribute::NoCapture);
        x_plvRtlCheckCast->arg_begin()[0].addAttr(llvm::Attribute::ReadOnly);
        x_plvRtlCheckCast->arg_begin()[1].addAttr(llvm::Attribute::ReadNone);
        // void UdcRtlCheckBound(int len, int idx);
        ty = llvm::FunctionType::get(x_ctx.tyVoid, {x_ctx.tyI32, x_ctx.tyI32}, false);
        x_plvRtlCheckBound = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("UdcRtlCheckBound", ty));
        x_plvRtlCheckBound->setCallingConv(llvm::CallingConv::C);
        x_plvRtlCheckBound->setDSOLocal(true);
        x_plvRtlCheckBound->setDoesNotRecurse();
        x_plvRtlCheckBound->setDoesNotThrow();
        x_plvRtlCheckBound->setLinkage(llvm::GlobalValue::ExternalLinkage);
        // int UdcRtlReadInteger();
        ty = llvm::FunctionType::get(x_ctx.tyI32, false);
        x_plvRtlReadInteger = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("UdcRtlReadInteger", ty));
        x_plvRtlReadInteger->setCallingConv(llvm::CallingConv::C);
        x_plvRtlReadInteger->setDSOLocal(true);
        x_plvRtlReadInteger->setDoesNotRecurse();
        x_plvRtlReadInteger->setDoesNotThrow();
        x_plvRtlReadInteger->setLinkage(llvm::GlobalValue::ExternalLinkage);
        // char *UdcRtlReadLine();
        ty = llvm::FunctionType::get(x_ctx.tyI8Ptr, false);
        x_plvRtlReadLine = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("UdcRtlReadLine", ty));
        x_plvRtlReadLine->setCallingConv(llvm::CallingConv::C);
        x_plvRtlReadLine->setDSOLocal(true);
        x_plvRtlReadLine->setDoesNotRecurse();
        x_plvRtlReadLine->setDoesNotThrow();
        x_plvRtlReadLine->setLinkage(llvm::GlobalValue::ExternalLinkage);
        x_plvRtlReadLine->setReturnDoesNotAlias();
        // int UdcRtlStrCmp(const char *lhs, const char *rhs);
        ty = llvm::FunctionType::get(x_ctx.tyI32, {x_ctx.tyI8Ptr, x_ctx.tyI8Ptr}, false);
        x_plvRtlStrCmp = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("UdcRtlStrCmp", ty));
        x_plvRtlStrCmp->setCallingConv(llvm::CallingConv::C);
        x_plvRtlStrCmp->setDSOLocal(true);
        x_plvRtlStrCmp->setDoesNotRecurse();
        x_plvRtlStrCmp->setDoesNotThrow();
        x_plvRtlStrCmp->setLinkage(llvm::GlobalValue::ExternalLinkage);
        x_plvRtlStrCmp->setOnlyReadsMemory();
        x_plvRtlStrCmp->arg_begin()[0].addAttr(llvm::Attribute::NoCapture);
        x_plvRtlStrCmp->arg_begin()[0].addAttr(llvm::Attribute::ReadOnly);
        x_plvRtlStrCmp->arg_begin()[1].addAttr(llvm::Attribute::NoCapture);
        x_plvRtlStrCmp->arg_begin()[1].addAttr(llvm::Attribute::ReadOnly);
    }
    // emit bool strlit
    {
        auto sFalse = X_EmitStringLiteral("false");
        auto sTrue = X_EmitStringLiteral("true");
        auto ty = llvm::ArrayType::get(x_ctx.tyI8Ptr, 2);
        auto gv = llvm::cast<llvm::GlobalVariable>(x_plvMod->getOrInsertGlobal("#BoolStr", ty));
        gv->setAlignment(x_ctx.lvDataLayout.getPrefTypeAlignment(ty));
        gv->setConstant(true);
        gv->setDSOLocal(true);
        gv->setInitializer(llvm::ConstantArray::get(ty, {sFalse, sTrue}));
        gv->setLinkage(llvm::GlobalValue::InternalLinkage);
        x_plvBoolStr = gv;
    }
    for (auto &upClass : vProg.GetClasses())
        upClass->AcceptVisitor(*this);
    // emit main function
    {
        // int main(void) {
        //     #Class.Main.main();
        //     return 0;
        // }
        auto ty = llvm::FunctionType::get(x_ctx.tyI32, false);
        auto fn = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("main", ty));
        fn->setDSOLocal(true);
        fn->setDoesNotThrow();
        fn->setLinkage(llvm::GlobalValue::ExternalLinkage);
        auto bb = llvm::BasicBlock::Create(x_ctx.lvCtx, "#fn-entry", fn);
        x_lvBld.SetInsertPoint(bb);
        auto pmain = vProg.GetMain();
        if (pmain)
            x_lvBld.CreateCall(pmain->GetLvFn());
        else
            PrintWarning(vProg.GetLocation(), "no entry point found");
        x_lvBld.CreateRet(llvm::ConstantInt::get(x_ctx.tyI32, 0));
        if (llvm::verifyFunction(*fn, &llvm::errs())) {
            Y_Reject();
            return;
        }
    }
    if (llvm::verifyModule(*x_plvMod, &llvm::errs())) {
        Y_Reject();
        return;
    }
}

void CodeGenVisitor::Visit(ClassDef &vClass) noexcept {
    ENTER_SCOPE(x_pClass, &vClass);
    for (auto &upField : vClass.GetFields())
        if (auto pFn = dynamic_cast<FnDef *>(upField.get()))
            pFn->AcceptVisitor(*this);
}

void CodeGenVisitor::Visit(FnDef &vFn) noexcept {
    llvm::Function *fn = vFn.GetLvFn();
    auto bb = llvm::BasicBlock::Create(x_ctx.lvCtx, "#fn-entry", fn);
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
    ENTER_SCOPE(x_uForStmtCnt, 0);
    ENTER_SCOPE(x_uIfStmtCnt, 0);
    ENTER_SCOPE(x_uWhileStmtCnt, 0);
    if (vFn.IsStatic())
        vFn.GetBody()->AcceptVisitor(*this);
    else {
        ENTER_SCOPE(x_plvThis, x_lvBld.CreatePointerCast(fn->arg_begin(), x_pClass->GetType().GetLvType()));
        vFn.GetBody()->AcceptVisitor(*this);
    }
    if (!x_lvBld.GetInsertBlock()->getTerminator()) {
        if (fn->getReturnType()->isVoidTy())
            x_lvBld.CreateRetVoid();
        else {
            Y_RjNoReturnVal(vFn.GetLocation(), vFn.GetType(), vFn.GetName());
            return;
        }
    }
    if (llvm::verifyFunction(*fn, &llvm::errs())) {
        Y_Reject();
        return;
    }
    x_plvFpm->run(*fn);
}

void CodeGenVisitor::Visit(VarDef &vVar) noexcept {
    assert(!vVar.IsField());
    if (x_lvBld.GetInsertBlock()->getTerminator()) {
        PrintWarning(vVar.GetLocation(), "unreachable code");
        return;
    }
    auto ptr = x_lvBld.CreateAlloca(vVar.GetType().GetLvType(), nullptr, vVar.GetName());
    vVar.SetLvPtr(ptr);
}

void CodeGenVisitor::Visit(BlockStmt &stmt) noexcept {
    if (x_lvBld.GetInsertBlock()->getTerminator()) {
        PrintWarning(stmt.GetLocation(), "unreachable code");
        return;
    }
    for (auto &upItem : stmt.GetItems())
        upItem->AcceptVisitor(*this);
}

void CodeGenVisitor::Visit(BreakStmt &stmt) noexcept {
    assert(x_plvPost);
    if (x_lvBld.GetInsertBlock()->getTerminator()) {
        PrintWarning(stmt.GetLocation(), "unreachable code");
        return;
    }
    x_lvBld.CreateBr(x_plvPost);
}

void CodeGenVisitor::Visit(ExprStmt &stmt) noexcept {
    if (x_lvBld.GetInsertBlock()->getTerminator()) {
        PrintWarning(stmt.GetLocation(), "unreachable code");
        return;
    }
    if (stmt.GetExpr())
        stmt.GetExpr()->AcceptVisitor(*this);
}

void CodeGenVisitor::Visit(ForStmt &stmt) noexcept {
    if (x_lvBld.GetInsertBlock()->getTerminator()) {
        PrintWarning(stmt.GetLocation(), "unreachable code");
        return;
    }
    auto fn = x_lvBld.GetInsertBlock()->getParent();
    auto bbCond = llvm::BasicBlock::Create(x_ctx.lvCtx, FmtStr("#for-cond.%u", x_uForStmtCnt));
    auto bbBody = llvm::BasicBlock::Create(x_ctx.lvCtx, FmtStr("#for-body.%u", x_uForStmtCnt));
    auto bbPost = llvm::BasicBlock::Create(x_ctx.lvCtx, FmtStr("#for-post.%u", x_uForStmtCnt));
    ++x_uForStmtCnt;
    if (stmt.GetInit())
        stmt.GetInit()->AcceptVisitor(*this);
    bbCond->insertInto(fn);
    x_lvBld.CreateBr(bbCond);
    x_lvBld.SetInsertPoint(bbCond);
    if (stmt.GetCond()) {
        stmt.GetCond()->AcceptVisitor(*this);
        x_lvBld.CreateCondBr(x_plvRet, bbBody, bbPost);
    }
    else
        x_lvBld.CreateBr(bbBody);
    bbBody->insertInto(fn);
    x_lvBld.SetInsertPoint(bbBody);
    ENTER_SCOPE(x_plvPost, bbPost);
    stmt.GetBody()->AcceptVisitor(*this);
    if (x_lvBld.GetInsertBlock()->getTerminator()) {
        if (stmt.GetIncr())
            PrintWarning(stmt.GetIncr()->GetLocation(), "unreachable code");
    }
    else {
        if (stmt.GetIncr())
            stmt.GetIncr()->AcceptVisitor(*this);
        x_lvBld.CreateBr(bbCond);
    }
    bbPost->insertInto(fn);
    if (llvm::pred_empty(bbPost))
        bbPost->eraseFromParent();
    else
        x_lvBld.SetInsertPoint(bbPost);
}

void CodeGenVisitor::Visit(IfStmt &stmt) noexcept {
    if (x_lvBld.GetInsertBlock()->getTerminator()) {
        PrintWarning(stmt.GetLocation(), "unreachable code");
        return;
    }
    auto fn = x_lvBld.GetInsertBlock()->getParent();
    auto bbThen = llvm::BasicBlock::Create(x_ctx.lvCtx, FmtStr("#if-then.%u", x_uIfStmtCnt));
    auto bbElse = stmt.GetElse() ?
        llvm::BasicBlock::Create(x_ctx.lvCtx, FmtStr("#if-else.%u", x_uIfStmtCnt)) :
        nullptr;
    auto bbPost = llvm::BasicBlock::Create(x_ctx.lvCtx, FmtStr("#if-post.%u", x_uIfStmtCnt));
    ++x_uIfStmtCnt;
    stmt.GetCond()->AcceptVisitor(*this);
    x_lvBld.CreateCondBr(x_plvRet, bbThen, stmt.GetElse() ? bbElse : bbPost);
    bbThen->insertInto(fn);
    x_lvBld.SetInsertPoint(bbThen);
    stmt.GetThen()->AcceptVisitor(*this);
    if (!x_lvBld.GetInsertBlock()->getTerminator())
        x_lvBld.CreateBr(bbPost);
    if (stmt.GetElse()) {
        bbElse->insertInto(fn);
        x_lvBld.SetInsertPoint(bbElse);
        stmt.GetElse()->AcceptVisitor(*this);
        if (!x_lvBld.GetInsertBlock()->getTerminator())
            x_lvBld.CreateBr(bbPost);
    }
    bbPost->insertInto(fn);
    if (llvm::pred_empty(bbPost))
        bbPost->eraseFromParent();
    else
        x_lvBld.SetInsertPoint(bbPost);
}

void CodeGenVisitor::Visit(PrintStmt &stmt) noexcept {
    if (x_lvBld.GetInsertBlock()->getTerminator()) {
        PrintWarning(stmt.GetLocation(), "unreachable code");
        return;
    }
    llvm::SmallVector<llvm::Value *, 64> vec {nullptr};
    llvm::SmallString<256> fmt;
    for (auto &upArg : stmt.GetArgs()) {
        upArg->AcceptVisitor(*this);
        auto val = x_plvRet;
        if (upArg->GetType() == x_pTyReg->tyInt) {
            fmt += "%d";
            vec.emplace_back(val);
        }
        else if (upArg->GetType() == x_pTyReg->tyBool) {
            fmt += "%s";
            val = x_lvBld.CreateIntCast(val, x_ctx.tySize, false);
            val = x_lvBld.CreateGEP(x_plvBoolStr, {llvm::ConstantInt::get(x_ctx.tySize, 0), val});
            vec.emplace_back(x_lvBld.CreateLoad(val));
        }
        else if (upArg->GetType() == x_pTyReg->tyString) {
            fmt += "%s";
            vec.emplace_back(val);
        }
    }
    vec[0] = X_EmitStringLiteral(fmt.c_str());
    x_lvBld.CreateCall(x_plvRtlPrintf, vec);
}

void CodeGenVisitor::Visit(ReturnStmt &stmt) noexcept {
    if (x_lvBld.GetInsertBlock()->getTerminator()) {
        PrintWarning(stmt.GetLocation(), "unreachable code");
        return;
    }
    if (stmt.GetExpr()) {
        stmt.GetExpr()->AcceptVisitor(*this);
        x_lvBld.CreateRet(x_lvBld.CreatePointerCast(
            x_plvRet, x_lvBld.GetInsertBlock()->getParent()->getReturnType()
        ));
    }
    else {
        x_lvBld.CreateRetVoid();
    }
}

void CodeGenVisitor::Visit(WhileStmt &stmt) noexcept {
    if (x_lvBld.GetInsertBlock()->getTerminator()) {
        PrintWarning(stmt.GetLocation(), "unreachable code");
        return;
    }
    auto fn = x_lvBld.GetInsertBlock()->getParent();
    auto bbCond = llvm::BasicBlock::Create(x_ctx.lvCtx, FmtStr("#while-cond.%u", x_uWhileStmtCnt));
    auto bbBody = llvm::BasicBlock::Create(x_ctx.lvCtx, FmtStr("#while-body.%u", x_uWhileStmtCnt));
    auto bbPost = llvm::BasicBlock::Create(x_ctx.lvCtx, FmtStr("#while-post.%u", x_uWhileStmtCnt));
    ++x_uWhileStmtCnt;
    x_lvBld.CreateBr(bbCond);
    bbCond->insertInto(fn);
    x_lvBld.SetInsertPoint(bbCond);
    stmt.GetCond()->AcceptVisitor(*this);
    x_lvBld.CreateCondBr(x_plvRet, bbBody, bbPost);
    bbBody->insertInto(fn);
    x_lvBld.SetInsertPoint(bbBody);
    ENTER_SCOPE(x_plvPost, bbPost);
    stmt.GetBody()->AcceptVisitor(*this);
    if (!x_lvBld.GetInsertBlock()->getTerminator())
        x_lvBld.CreateBr(bbCond);
    bbPost->insertInto(fn);
    x_lvBld.SetInsertPoint(bbPost);
}

void CodeGenVisitor::Visit(AssignExpr &expr) noexcept {
    expr.GetRhs()->AcceptVisitor(*this);
    auto val = x_plvRet;
    expr.GetLhs()->SetGenPtr(true);
    expr.GetLhs()->AcceptVisitor(*this);
    auto ptr = x_plvRet;
    x_lvBld.CreateStore(x_lvBld.CreateBitOrPointerCast(val, expr.GetLhs()->GetType().GetLvType()), ptr);
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
        if (expr.GetLhs()->GetType() == x_pTyReg->tyString) {
            lhs = x_lvBld.CreateCall(x_plvRtlStrCmp, {lhs, rhs});
            rhs = llvm::ConstantInt::get(x_ctx.tyI32, 0);
        }
        x_plvRet = !lhs->getType()->isPointerTy() ? x_lvBld.CreateICmpEQ(lhs, rhs) :
            x_plvRet = x_lvBld.CreateICmpEQ(
                x_lvBld.CreatePointerCast(lhs, x_ctx.tyVoidPtr),
                x_lvBld.CreatePointerCast(rhs, x_ctx.tyVoidPtr)
            );
        break;
    case BinOp::kNeq:
        if (expr.GetLhs()->GetType() == x_pTyReg->tyString) {
            lhs = x_lvBld.CreateCall(x_plvRtlStrCmp, {lhs, rhs});
            rhs = llvm::ConstantInt::get(x_ctx.tyI32, 0);
        }
        x_plvRet = !lhs->getType()->isPointerTy() ? x_lvBld.CreateICmpNE(lhs, rhs) :
            x_plvRet = x_lvBld.CreateICmpNE(
                x_lvBld.CreatePointerCast(lhs, x_ctx.tyVoidPtr),
                x_lvBld.CreatePointerCast(rhs, x_ctx.tyVoidPtr)
            );
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
        x_plvRet = x_lvBld.CreateNSWAdd(lhs, rhs);
        break;
    case BinOp::kSub:
        x_plvRet = x_lvBld.CreateNSWSub(lhs, rhs);
        break;
    case BinOp::kMul:
        x_plvRet = x_lvBld.CreateNSWMul(lhs, rhs);
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
    if (expr.IsArrayLength())
        x_plvRet = x_lvBld.CreateLoad(x_lvBld.CreateStructGEP(who, 0));
    else {
        auto &vFn = expr.GetFn();
        llvm::SmallVector<llvm::Value *, 64> vec;
        if (!vFn.IsStatic())
            vec.emplace_back(x_lvBld.CreatePointerCast(who, x_ctx.tyVoidPtr));
        auto itPar = expr.GetFn().GetPars().begin();
        for (auto &upArg : expr.GetArgs()) {
            upArg->AcceptVisitor(*this);
            vec.emplace_back(x_lvBld.CreateBitOrPointerCast(x_plvRet, (*itPar)->GetType().GetLvType()));
            ++itPar;
        }
        if (vFn.IsStatic())
            x_plvRet = x_lvBld.CreateCall(vFn.GetLvFn(), vec);
        else {
            // virtual call
            // fn **vtable = *(fn ***) who;
            // fn *pfn = vtable[idx + 1];
            // pfn(args...);
            auto ty = vFn.GetLvFn()->getFunctionType();
            auto typpp = ty->getPointerTo()->getPointerTo()->getPointerTo();
            auto vtable = x_lvBld.CreateLoad(x_lvBld.CreatePointerCast(who, typpp));
            auto idx = llvm::ConstantInt::get(x_ctx.tySize, vFn.GetIdx() + 1);
            auto pfn = x_lvBld.CreateLoad(x_lvBld.CreateGEP(vtable, idx));
            x_plvRet = x_lvBld.CreateCall(ty, pfn, vec);
        }
    }
}

void CodeGenVisitor::Visit(CastExpr &expr) noexcept {
    expr.GetExpr()->AcceptVisitor(*this);
    if (!expr.GetType().Accepts(expr.GetExpr()->GetType())) {
        // dynamic cast
        auto who = x_lvBld.CreatePointerCast(x_plvRet, x_ctx.tyVoidPtr);
        auto i8p = llvm::ConstantExpr::getPointerCast(x_plvClassIdx, x_ctx.tyI8Ptr);
        auto iidx = static_cast<const ClassDef *>(&expr.GetType().GetElemType())->GetIdx();
        auto ciIdx = llvm::ConstantInt::get(x_ctx.tySize, iidx * x_ctx.uPtrSize);
        auto idx = llvm::ConstantExpr::getGetElementPtr(x_ctx.tyI8, i8p, ciIdx);
        x_lvBld.CreateCall(x_plvRtlCheckCast, {who, idx});
    }
    // static cast
    x_plvRet = x_lvBld.CreatePointerCast(x_plvRet, expr.GetType().GetLvType());
}

void CodeGenVisitor::Visit(NewArrayExpr &expr) noexcept {
    expr.GetExpr()->AcceptVisitor(*this);
    auto len = x_plvRet;
    auto ty = llvm::cast<llvm::PointerType>(expr.GetType().GetLvType());
    auto tyElem = x_pTyReg->DecDim(expr.GetType()).GetLvType();
    auto cbOff = llvm::ConstantInt::get(x_ctx.tySize, x_ctx.lvDataLayout.getTypeAllocSize(ty->getElementType()));
    auto cbElem = llvm::ConstantInt::get(x_ctx.tySize, x_ctx.lvDataLayout.getTypeAllocSize(tyElem));
    auto ptr = x_lvBld.CreatePointerCast(x_lvBld.CreateCall(x_plvRtlAllocArray, {len, cbOff, cbElem}), ty);
    x_lvBld.CreateStore(len, x_lvBld.CreateStructGEP(ptr, 0));
    x_plvRet = ptr;
}

void CodeGenVisitor::Visit(NewClassExpr &expr) noexcept {
    auto ty = llvm::cast<llvm::PointerType>(expr.GetType().GetLvType());
    auto cb = llvm::ConstantInt::get(x_ctx.tySize, x_ctx.lvDataLayout.getTypeAllocSize(ty->getElementType()));
    auto ptr = x_lvBld.CreatePointerCast(x_lvBld.CreateCall(x_plvRtlAlloc, {cb}), ty);
    auto vtable = x_lvBld.CreatePointerCast(ptr, x_ctx.tyVoidPtrPtr);
    auto pClass = static_cast<const ClassDef *>(&expr.GetType().GetElemType());
    x_lvBld.CreateStore(llvm::ConstantExpr::getPointerCast(pClass->GetLvVTable(), x_ctx.tyVoidPtr), vtable);
    x_plvRet = ptr;
}

void CodeGenVisitor::Visit(UnaryExpr &expr) noexcept {
    expr.GetExpr()->AcceptVisitor(*this);
    switch (expr.GetOp()) {
    case UnaOp::kNeg:
        x_plvRet = x_lvBld.CreateNSWNeg(x_plvRet);
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
    auto idx = x_plvRet;
    auto len = x_lvBld.CreateLoad(x_lvBld.CreateStructGEP(ptr, 0));
    x_lvBld.CreateCall(x_plvRtlCheckBound, {len, idx});
    idx = x_lvBld.CreateIntCast(idx, x_ctx.tySize, false);
    ptr = x_lvBld.CreateGEP(
        ptr,
        {llvm::ConstantInt::get(x_ctx.tySize, 0), llvm::ConstantInt::get(x_ctx.tyI32, 1), idx}
    );
    x_plvRet = expr.IsGenPtr() ? ptr : x_lvBld.CreateLoad(ptr);
}

void CodeGenVisitor::Visit(InstanceOf &expr) noexcept {
    expr.GetExpr()->AcceptVisitor(*this);
    auto who = x_lvBld.CreatePointerCast(x_plvRet, x_ctx.tyVoidPtr);
    auto i8p = llvm::ConstantExpr::getPointerCast(x_plvClassIdx, x_ctx.tyI8Ptr);
    auto ciIdx = llvm::ConstantInt::get(x_ctx.tySize, expr.GetClass().GetIdx() * x_ctx.uPtrSize);
    auto idx = llvm::ConstantExpr::getGetElementPtr(x_ctx.tyI8, i8p, ciIdx);
    x_plvRet = x_lvBld.CreateCall(x_plvRtlInstanceOf, {who, idx});
}

void CodeGenVisitor::Visit(ReadInteger &expr) noexcept {
    x_plvRet = x_lvBld.CreateCall(x_plvRtlReadInteger);
}

void CodeGenVisitor::Visit(ReadLine &expr) noexcept {
    x_plvRet = x_lvBld.CreateCall(x_plvRtlReadLine);
}

void CodeGenVisitor::Visit(This &expr) noexcept {
    x_plvRet = x_plvThis;
}

void CodeGenVisitor::Visit(VarAccess &expr) noexcept {
    auto pClass = (const ClassDef *) x_pClass;
    auto who = static_cast<llvm::Value *>(x_plvThis);
    if (expr.GetExpr()) {
        expr.GetExpr()->AcceptVisitor(*this);
        pClass = static_cast<const ClassDef *>(&expr.GetExpr()->GetType().GetElemType());
        who = x_plvRet;
    }
    if (expr.GetVar().IsField()) {
        llvm::SmallVector<llvm::Value *, 64> vec {llvm::ConstantInt::get(x_ctx.tySize, 0)};
        while (pClass != &expr.GetVar().GetClass()) {
            pClass = pClass->GetBase();
            vec.emplace_back(llvm::ConstantInt::get(x_ctx.tyI32, 0));
        }
        vec.emplace_back(llvm::ConstantInt::get(x_ctx.tyI32, expr.GetVar().GetIdx() + 1));
        who = x_lvBld.CreateGEP(who, vec);
    }
    else
        who = expr.GetVar().GetLvPtr();
    assert(who);
    x_plvRet = expr.IsGenPtr() ? who : x_lvBld.CreateLoad(who);
}

void CodeGenVisitor::Visit(BoolLit &expr) noexcept {
    x_plvRet = expr.GetVal() ? llvm::ConstantInt::getTrue(x_ctx.tyI1) : llvm::ConstantInt::getFalse(x_ctx.tyI1);
}

void CodeGenVisitor::Visit(IntLit &expr) noexcept {
    x_plvRet = llvm::ConstantInt::get(x_ctx.tyI32, expr.GetVal());
}

void CodeGenVisitor::Visit(NullLit &expr) noexcept {
    x_plvRet = x_ctx.ptrVoidNull;
}

void CodeGenVisitor::Visit(StrLit &expr) noexcept {
    x_plvRet = X_EmitStringLiteral(expr.GetVal().c_str());
}

}
